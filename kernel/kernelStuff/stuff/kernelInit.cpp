#include "kernelInit.h"
#include "../../paging/PageTableManager.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include "../../devices/serial/serial.h"
//#include "../../devices/gdt/gdt.h"
#include "../../devices/gdt/gdt2.h"
#include "../../devices/rtc/rtc.h"
#include "../../devices/pit/pit.h"
#include "../../interrupts/IDT.h"
#include "../../interrupts/interrupts.h"
#include "../IO/IO.h"
#include "../../osData/MStack/MStackM.h"
#include "../../memory/heap.h"
#include "../../scheduler/scheduler.h"
#include "../../devices/gdt/initialGdt.h"
#include "../../devices/keyboard/keyboard.h"
#include "../../devices/mouse/mouse.h"
#include "../../devices/pci/pci.h"
#include "../../diskStuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../../diskStuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../../diskStuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
#include <libm/cstr.h>
#include "../../audio/audioDevStuff.h"


BasicRenderer tempRenderer = BasicRenderer(NULL, NULL);

void InitKernel(BootInfo* bootInfo)
{
    _memset(bootInfo->framebuffer->BaseAddress, 0x00, bootInfo->framebuffer->BufferSize);

    tempRenderer = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
    GlobalRenderer = &tempRenderer;

    Serial::pciCard = 0;
    Serial::Init();

    PrepBootScreen();
    PrintMsg("> Initializing Kernel");
    Println();
    PrintMsgStartLayer("BOOT");
    StepDone(0);

    PrintMsg("> Preparing Memory");
    PrepareMemory(bootInfo);
    StepDone();
    
    PrintMsg("> Preparing GDT");
    //InitInitialGdt();
    DoGdtStuff();
    StepDone();
    
    PrintMsg("> Preparing Interrupts");
    PrepareInterrupts();
    StepDone();
    

    PrintMsg("> Initializing Heap");
    InitializeHeap((void*)MEM_AREA_KERNEL_HEAP_START, 0x10);
    StepDone();

    PrintMsg("> Initing RTC");
    RTC::InitRTC();
    StepDone();
    PrintMsg("> Reading RTC");
    RTC::read_rtc();
    StepDone();

    PrintMsg("> Updating RTC Time");
    RTC::UpdateTimeIfNeeded();
    StepDone();

    PrintMsgStartLayer("RTC Info");
    // PrintMsgColSL("TIME: ", Colors.yellow);
    // PrintMsgColSL("{}:", to_string((int)RTC::Hour), Colors.yellow);
    // PrintMsgColSL("{}:", to_string((int)RTC::Minute), Colors.yellow);
    // PrintMsgCol("{}", to_string((int)RTC::Second), Colors.yellow);
    PrintfMsgCol("TIME: %d:%d:%d", Colors.yellow, RTC::Hour, RTC::Minute, RTC::Second);
    
    // PrintMsgColSL("DATE: ", Colors.yellow);
    // PrintMsgColSL("{}.", to_string((int)RTC::Day), Colors.yellow);
    // PrintMsgColSL("{}.", to_string((int)RTC::Month), Colors.yellow);
    // PrintMsgCol("{}", to_string((int)RTC::Year), Colors.yellow);
    PrintfMsgCol("DATE: %d.%d.%d", Colors.yellow, RTC::Day, RTC::Month, RTC::Year);
    PrintMsgEndLayer("RTC Info");

    PrintMsg("> Initing PIT");
    PIT::InitPIT();
    StepDone();

    #define STAT 0x64
    #define CMD 0x60
    
    PrintMsg("> Clearing Input Buffer (1/2)");
    {
        // Clear the input buffer.
        size_t timeout = 1024;
        while ((inb(STAT) & 1) && timeout > 0) {
            timeout--;
            inb(CMD);
        }
    }
    StepDone();
    
    PrintMsg("> Initing PS/2 Mouse");
    Mouse::InitPS2Mouse();//(bootInfo->mouseZIP, "default.mbif");
    //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));
    StepDone();

    PrintMsg("> Initing Keyboard");
    Keyboard::InitKeyboard();
    StepDone();

//     PrintMsg("> Initing PS/2 Keyboard");
//     InitKeyboard();
//     StepDone();
    
    PrintMsg("> Initialising Scheduler");
    Scheduler::InitScheduler();
    StepDone();

    PrintMsg("> Preparing Interrupts (2)");
    PrepareInterrupts2();
    PIT::Inited = true;
    StepDone();


    PrintMsg("> Clearing Input Buffer (2/2)");
    {
        // Clear the input buffer.
        size_t timeout = 1024;
        while ((inb(STAT) & 1) && timeout > 0) {
            timeout--;
            inb(CMD);
        }
    }
    StepDone();



    PrintMsg("> Creating OS Ram Disk");
    PrintMsgStartLayer("OS RAM DISK");
    AddToStack();
    {

        uint64_t totSize = 500000;

        totSize += bootInfo->bootImage->size;
        totSize += bootInfo->programs->size;

        totSize += 6000000;
        uint64_t totSize2 = totSize;
        totSize2 /= 512;
        totSize -= 200000;

        AddToStack();
        DiskInterface::RamDiskInterface* ramDisk = new DiskInterface::RamDiskInterface(totSize2);
        RemoveFromStack();

        AddToStack();
        PrintMsg("> Creating List for Disk Interfaces");
        osData.diskInterfaces = List<DiskInterface::GenericDiskInterface*>(4);
        RemoveFromStack();

        AddToStack();
        PrintMsg("> Creating List for Audio Destinations");
        //osData.audioDestinations = List<Audio::BasicAudioDestination*>();
        osData.audioInputDevices = List<Audio::AudioInputDevice*>();
        osData.audioOutputDevices = List<Audio::AudioOutputDevice*>();

        int pcSpeakerFreq = PIT::BaseFrequency / AudioDeviceStuff::rawAudioDiv;
        int pcSpeakerSampleCount = (pcSpeakerFreq + 9) / 10;

        osData.pcSpeakerDev = new Audio::AudioOutputDevice("PC Speaker", new Audio::AudioBuffer(8, pcSpeakerFreq, 1, pcSpeakerSampleCount));
        osData.defaultAudioOutputDevice = osData.pcSpeakerDev;
        AudioDeviceStuff::pcSpk = osData.pcSpeakerDev;
        AudioDeviceStuff::pcSpk->destination->buffer->ClearBuffer();
        AudioDeviceStuff::pcSpk->destination->buffer->sampleCount = AudioDeviceStuff::pcSpk->destination->buffer->totalSampleCount;
        AudioDeviceStuff::init();
        RemoveFromStack();

        AddToStack();
        osData.diskInterfaces.Add(ramDisk);
        RemoveFromStack();

        AddToStack();
        DiskInterface::GenericDiskInterface* diskInterface = ramDisk;
        PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)new PartitionInterface::MRAPSPartitionInterface(diskInterface);
        partInterface->InitAndSavePartitionTable();
        RemoveFromStack();

        AddToStack();
        partInterface->CreatePartition(partInterface->partitionList[2], totSize);

        partInterface->partitionList[2]->driveName=StrCopy("bruh");
        partInterface->partitionList[2]->driveNameLen=StrLen("bruh");
        partInterface->SavePartitionTable();
        RemoveFromStack();

        AddToStack();
        PrintfMsg("> PART: %X", partInterface);
        FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)new FilesystemInterface::MrafsFilesystemInterface(partInterface, partInterface->partitionList[2]);
        RemoveFromStack();


        AddToStack();
        fsInterface->InitAndSaveFSTable();
        RemoveFromStack();

        
        PrintMsgStartLayer("PROGRAMS");
        {
            fsInterface->CreateFolder("programs");
        }
        PrintMsgEndLayer("PROGRAMS");

        PrintMsgStartLayer("MODULES");
        {
            fsInterface->CreateFolder("modules");
        }
        PrintMsgEndLayer("MODULES");

        fsInterface->SaveFSTable();

        //while (true);

    }
    RemoveFromStack();
    PrintMsgEndLayer("OS RAM DISK");
    StepDone();

    AddToStack();
    PrintMsg("> Prepare ACPI");
    PrintMsgStartLayer("ACPI");
    PrepareACPI(bootInfo);
    PrintMsgEndLayer("ACPI");
    PrintDebugTerminal();
    StepDone();
    RemoveFromStack();






    PrintMsgEndLayer("BOOT");

    StepDone();
    // if (PIT::TicksSinceBoot != 0)
    //     PIT::Sleep(200);

    Println();
    PrintMsgCol("> Inited Kernel!", Colors.bgreen);
}

#define IDT_SIZE	256

#define IDT_TYPE_PRESENT	0b1
#define IDT_TYPE_PRIVILEGE	0b00

#define IDT_IST_ISR		1
#define IDT_IST_IRQ		2
#define IDT_IST_TIMER	3

/**
 * Enable the FPU
 *
 * We are assuming that we have one to begin with, but since we
 * only really operate on 686 machines, we do, so we're not
 * going to bother checking.
 */

void set_fpu_cw(const uint16_t cw) 
{
	asm volatile("fldcw %0" :: "m"(cw));
}

void enable_fpu() 
{
	size_t cr4;
	asm volatile ("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200;
	asm volatile ("mov %0, %%cr4" :: "r"(cr4));
	set_fpu_cw(0x37F);
}

void DoGdtStuff()
{
    //GlobalRenderer->Clear(Colors.red);
    //void* x = GlobalAllocator->RequestPage();
    GDTBlock* gdt_block = (GDTBlock*)GlobalAllocator->RequestPage();
    //while (true);
    //GlobalRenderer->Clear(Colors.green);
    GlobalPageTableManager.MapMemory(gdt_block, gdt_block);
    //GlobalRenderer->Clear(Colors.blue);
    PrintfMsg(" > GDT: %X", (uint64_t)gdt_block);
    
    //while (true);

    char* stack_kernel = (char*)GlobalAllocator->RequestPages(8);
    char* stack_kernel_end = stack_kernel + 0x1000 * 8;
    GlobalPageTableManager.MapMemories(stack_kernel, stack_kernel, 8);

    char* stack_isr = (char*)GlobalAllocator->RequestPages(8);
    char* stack_isr_end = stack_isr + 0x1000 * 8;
    GlobalPageTableManager.MapMemories(stack_isr, stack_isr, 8);

    char* stack_irq = (char*)GlobalAllocator->RequestPages(8);
    char* stack_irq_end = stack_irq + 0x1000 * 8;
    GlobalPageTableManager.MapMemories(stack_irq, stack_irq, 8);

    char* stack_timer = (char*)GlobalAllocator->RequestPages(8);
    char* stack_timer_end = stack_timer + 0x1000 * 8;
    GlobalPageTableManager.MapMemories(stack_timer, stack_timer, 8);

    //while (true);

    gdt_init(gdt_block);
	gdt_set_tss_ring(gdt_block, 0, stack_kernel_end);
	gdt_set_tss_ist(gdt_block, IDT_IST_ISR, stack_isr_end);
	gdt_set_tss_ist(gdt_block, IDT_IST_IRQ, stack_irq_end);
	gdt_set_tss_ist(gdt_block, IDT_IST_TIMER, stack_timer_end);
	gdt_load(&gdt_block->gdt_descriptor);


    enable_fpu();
    //cpu_enable_features();
}


PageFrameAllocator tempAllocator;

#define USER_END            0x0000007fffffffff
#define TRANSLATED_PHYSICAL_MEMORY_BEGIN 0xffff800000000000llu
#define MMIO_BEGIN          0xffffffff00000000llu
#define KERNEL_TEMP_BEGIN   0xffffffff40000000llu
#define KERNEL_DATA_BEGIN   0xffffffff80000000llu
#define KERNEL_HEAP_BEGIN   0xffffffff80300000llu

static inline uint64_t earlyVirtualToPhysicalAddr(const void* vAddr)
{
    if((0xfffff00000000000llu & (uint64_t)vAddr) == TRANSLATED_PHYSICAL_MEMORY_BEGIN)
        return ~TRANSLATED_PHYSICAL_MEMORY_BEGIN & (uint64_t)vAddr;
    else
        return ~KERNEL_DATA_BEGIN & (uint64_t)vAddr;
}



void PrepareMemory(BootInfo* bootInfo)
{
    GlobalAllocator = &tempAllocator;
    PrintMsgStartLayer("EFI Memory Map");
    GlobalAllocator->ReadEFIMemoryMap(bootInfo->m2MapStart, bootInfo->mMapSize); 
    PrintMsgEndLayer("EFI Memory Map");
    

    //uint64_t kernelSize = (((uint64_t)&_KernelEnd) - ((uint64_t)&_KernelStart));
    //uint64_t kernelPages = ((uint64_t)kernelSize / 4096) + 1;

    //GlobalAllocator->LockPages(&_KernelStart, kernelPages);

    uint64_t stackPointer = 0;
    uint64_t stackPageCount = 16;
    asm volatile("mov %%rsp, %0" : "=r"(stackPointer));
    stackPointer &= 0xfffffffffffff000;
    stackPointer -= 0x1000 * stackPageCount;
    stackPageCount += 4;

    uint64_t bootInfoAddr = (uint64_t)bootInfo;

    uint64_t rFB = earlyVirtualToPhysicalAddr(GlobalRenderer->framebuffer->BaseAddress);

    PageTable* PML4 = (PageTable*)GlobalAllocator->RequestPage();
    //GlobalPageTableManager.MapMemory(PML4, PML4);
    _memset(PML4, 0, 0x1000);
    GlobalPageTableManager = PageTableManager(PML4);
    PrintMsg("> Getting PML4 Stuff");
    //GlobalRenderer->Println("PML4 ADDR:          {}", ConvertHexToString((uint64_t)PML4), Colors.yellow);
    //asm volatile("mov %%cr3, %0" : "=r"(PML4));
    // asm("mov %0, %%cr3" : : "r" (PML4) );
    PrintMsgStartLayer("Info");
    PrintfMsgCol("PML4 ADDR:          %X", Colors.yellow, (uint64_t)PML4);
    PrintfMsgCol("ALLOC ADDR:         %X", Colors.yellow, GlobalAllocator->EFI_BITMAP_START + GlobalAllocator->EFI_BITMAP_SIZE);
    PrintfMsgCol("FB 1 ADDR:          %X", Colors.yellow, rFB);
    PrintfMsgCol("FB 2 ADDR:          %X", Colors.yellow, (uint64_t)GlobalRenderer->framebuffer->BaseAddress);
    PrintfMsgCol("FB 3 ADDR:          %X", Colors.yellow, (uint64_t)bootInfo->framebuffer->BaseAddress);
    PrintfMsgCol("MMAP ADDR:          %X", Colors.yellow, (uint64_t)bootInfo->mMapStart);
    PrintfMsgCol("MMAP2 ADDR:         %X", Colors.yellow, (uint64_t)bootInfo->m2MapStart);
    PrintfMsgCol("KERNEL ADDR:        %X", Colors.yellow, (uint64_t)bootInfo->kernelStart);
    PrintfMsgCol("KERNEL ADDR 2:      %X", Colors.yellow, earlyVirtualToPhysicalAddr(bootInfo->kernelStart));
    PrintfMsgCol("KERNEL V ADDR:      %X", Colors.yellow, (uint64_t)bootInfo->kernelStartV);
    PrintfMsgCol("KERNEL SIZE:        %X", Colors.yellow, (uint64_t)bootInfo->kernelSize);
    PrintfMsgCol("KERNEL END:         %X", Colors.yellow, (uint64_t)bootInfo->kernelStart + (uint64_t)bootInfo->kernelSize);
    PrintfMsgCol("KERNEL V END:       %X", Colors.yellow, (uint64_t)bootInfo->kernelStartV + (uint64_t)bootInfo->kernelSize);
    PrintfMsgCol("STACK BASE ADDR:    %X", Colors.yellow, earlyVirtualToPhysicalAddr((void*)(stackPointer - stackPageCount * 0x1000)));
    PrintfMsgCol("STACK ADDR:         %X", Colors.yellow, earlyVirtualToPhysicalAddr((void*)stackPointer));
    PrintfMsgCol("STACK V ADDR:       %X", Colors.yellow, stackPointer);
    PrintfMsgCol("STACK SIZE:         %X", Colors.yellow, stackPageCount * 0x1000);
    PrintfMsgCol("BOOT INFO ADDR:     %X", Colors.yellow, bootInfoAddr);
    PrintfMsgCol("RSDP ADDR:          %X", Colors.yellow, (uint64_t)bootInfo->rsdp);
    PrintfMsgCol("PROG ADDR:          %X", Colors.yellow, (uint64_t)bootInfo->programs);
    PrintMsgEndLayer("Info");

    //while (true);
    //PrintMsg("");

    PrintMsgStartLayer("Important Addr");
    PrintfMsg("FB: %X", (uint64_t)&GlobalRenderer->framebuffer);
    PrintfMsg("FB->buff: %X", (uint64_t)&GlobalRenderer->framebuffer->BaseAddress);
    PrintfMsg("psf1_Font: %X", (uint64_t)&GlobalRenderer->psf1_font);
    PrintfMsg("PSF1_Font->glyph: %X", (uint64_t)&GlobalRenderer->psf1_font->glyphBuffer);
    PrintfMsg("PSF1_Font->psf1_Header: %X", (uint64_t)&GlobalRenderer->psf1_font->psf1_Header);
    PrintMsgEndLayer("Important Addr");

    //while (true);

    // Map the efi memory things
    PrintMsgStartLayer("EFI Entries");
    {
        for (int i = 0; i < bootInfo->memEntryCount; i++)
        {
            MEM_MAP_ENTRY* entry = bootInfo->memEntries[i];
            // if (entry->type != 6)
            //     continue;
            uint64_t entryStartReal = entry->base;
            uint64_t entryStartVirt = entry->base | 0xffff800000000000; // ffff80007f5f4003
            uint64_t entrySize = entry->length;
            uint64_t entryPageCount = (entrySize + 0xFFF) / 0x1000;
            
            PrintfMsg("> Entry %d: %X -> %X (%d pages)", i, entryStartVirt, entryStartReal, entryPageCount);

            
            GlobalPageTableManager.MapMemories((void*)entryStartReal, (void*)entryStartReal, entryPageCount);
            GlobalPageTableManager.MapMemories((void*)entryStartVirt, (void*)entryStartReal, entryPageCount);
        }
    }
    PrintMsgEndLayer("EFI Entries");

    // Map Kernel
    {
        uint64_t kernelStartReal = (uint64_t)bootInfo->kernelStart;
        uint64_t kernelStartVirtual = (uint64_t)bootInfo->kernelStartV;
        uint64_t kernelStartVirtual2 = kernelStartReal | 0xffff800000000000;
        uint64_t kernelPageCount = (bootInfo->kernelSize + 0xFFF) / 0x1000;
        GlobalPageTableManager.MapMemories((void*)kernelStartVirtual, (void*)kernelStartReal, kernelPageCount);
        GlobalPageTableManager.MapMemories((void*)kernelStartVirtual2, (void*)kernelStartReal, kernelPageCount);
    }

    // Map Stack
    {
        uint64_t stackBaseReal = (uint64_t)earlyVirtualToPhysicalAddr((void*)stackPointer);
        uint64_t stackBaseVirtual = stackPointer;
        // Serial::Writelnf("ADDR: %X", stackPointer);
        // Serial::Writelnf("ADDR: %X", stackBaseVirtual);
        // Serial::Writelnf("ADDR: %X", stackPageCount);
        GlobalPageTableManager.MapMemories((void*)stackBaseVirtual, (void*)stackBaseReal, stackPageCount);
    }

    // Map the starting 0x1000 pages of TRANSLATED_PHYSICAL_MEMORY_BEGIN
    {
        uint64_t startVirtual = TRANSLATED_PHYSICAL_MEMORY_BEGIN;
        uint64_t startReal = earlyVirtualToPhysicalAddr((void*)TRANSLATED_PHYSICAL_MEMORY_BEGIN);
        uint64_t pageCount = 0x1000;
        GlobalPageTableManager.MapMemories((void*)startVirtual, (void*)startReal, pageCount);
    }

    // Map framebuffer
    {
        uint64_t fbBaseReal = (uint64_t)rFB;
        uint64_t fbBaseVirtual = (uint64_t)bootInfo->framebuffer->BaseAddress;
        uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize;
        uint64_t fbPageCount = (fbSize + 0xFFF) / 0x1000;
        GlobalPageTableManager.MapMemories((void*)fbBaseVirtual, (void*)fbBaseReal, fbPageCount, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_WriteThrough | PT_Flag_CacheDisabled);
    }

    asm("mov %0, %%cr3" : : "r" (PML4) );

}


uint8_t testIdtrArr[0x1000];
IDTR idtr;

void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector)
{
    IDTDescEntry* interrupt = (IDTDescEntry*)(idtr.Offset + entryOffset * sizeof(IDTDescEntry));
    interrupt->SetOffset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}

void PrepareInterrupts()
{  
    idtr.Limit = 0x1000 - 1;

    for (int i = 0; i < 0x1000; i++)
        testIdtrArr[i] = 0;

    idtr.Offset = (uint64_t)testIdtrArr;//(uint64_t)GlobalAllocator->RequestPage();

    // GenericInt_handler
    for (int i = 0; i < (0x1000 / sizeof(IDTDescEntry)); i++)
        SetIDTGate((void*)intr_stub_254, i, IDT_TA_InterruptGate, 0x08);

    for (int i = 0; i < 256; i++)
    {
        IRQHandlerCallbackHelpers[i] = NULL;
        IRQHandlerCallbackFuncs[i] = NULL;
    }

    // Main Stuff
    SetIDTGate((void*)intr_stub_14, 0xE, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)intr_stub_8, 0x8, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)intr_stub_13, 0xD, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)KeyboardInt_handler, 0x21, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)MouseInt_handler, 0x2C, IDT_TA_InterruptGate, 0x08);
    //SetIDTGate((void*)PITInt_handler, 0x20, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)intr_stub_32, 0x20, IDT_TA_InterruptGate, 0x08);

    // Main Exceptions
    SetIDTGate((void*)intr_stub_0, 0x0, IDT_TA_InterruptGate, 0x08); // Divide by 0
    SetIDTGate((void*)intr_stub_1, 0x1, IDT_TA_InterruptGate, 0x08); // Debug
    SetIDTGate((void*)intr_stub_3, 0x3, IDT_TA_InterruptGate, 0x08); // Breakpoint
    SetIDTGate((void*)intr_stub_16, 0x10, IDT_TA_InterruptGate, 0x08); // x87 Float error
    SetIDTGate((void*)intr_stub_19, 0x13, IDT_TA_InterruptGate, 0x08); // SIMD Float error

    // Other Exceptions
    SetIDTGate((void*)intr_stub_2, 0x2, IDT_TA_InterruptGate, 0x08); // Non Maskable interrupt
    SetIDTGate((void*)intr_stub_4, 0x4, IDT_TA_InterruptGate, 0x08); // Overflow
    SetIDTGate((void*)intr_stub_5, 0x5, IDT_TA_InterruptGate, 0x08); // Bound Range Exceeded
    SetIDTGate((void*)intr_stub_6, 0x6, IDT_TA_InterruptGate, 0x08); // Invalid OPCODE
    SetIDTGate((void*)intr_stub_7, 0x7, IDT_TA_InterruptGate, 0x08); // Device not avaiable
    SetIDTGate((void*)intr_stub_10, 0xA, IDT_TA_InterruptGate, 0x08); // Invalid TSS
    SetIDTGate((void*)intr_stub_11, 0xB, IDT_TA_InterruptGate, 0x08); // Segment not present
    SetIDTGate((void*)intr_stub_12, 0xC, IDT_TA_InterruptGate, 0x08); // Stack segment fault
    SetIDTGate((void*)intr_stub_17, 0x11, IDT_TA_InterruptGate, 0x08); //  Alligment check
    SetIDTGate((void*)intr_stub_18, 0x12, IDT_TA_InterruptGate, 0x08); // machine check
    SetIDTGate((void*)intr_stub_20, 0x14, IDT_TA_InterruptGate, 0x08); // Virtualization Exception
    SetIDTGate((void*)intr_stub_21, 0x15, IDT_TA_InterruptGate, 0x08); // Control Protection Exception
    SetIDTGate((void*)intr_stub_28, 0x1C, IDT_TA_InterruptGate, 0x08); // Hypervisor Injection Exception
    SetIDTGate((void*)intr_stub_29, 0x1D, IDT_TA_InterruptGate, 0x08); // VMM Communication Exception
    SetIDTGate((void*)intr_stub_30, 0x1E, IDT_TA_InterruptGate, 0x08); // Security Exception

    // Unhandled IRQs
    //SetIDTGate((void*)IRQ0_handler, 0x20, IDT_TA_InterruptGate, 0x08); // IRQ0 Handled
    //SetIDTGate((void*)IRQ1_handler, 0x21, IDT_TA_InterruptGate, 0x08); // IRQ1 Handled
    SetIDTGate((void*)IRQ2_handler, 0x22, IDT_TA_InterruptGate, 0x08); // IRQ2
    SetIDTGate((void*)IRQ3_handler, 0x23, IDT_TA_InterruptGate, 0x08); // IRQ3
    SetIDTGate((void*)IRQ4_handler, 0x24, IDT_TA_InterruptGate, 0x08); // IRQ4
    SetIDTGate((void*)IRQ5_handler, 0x25, IDT_TA_InterruptGate, 0x08); // IRQ5
    SetIDTGate((void*)IRQ6_handler, 0x26, IDT_TA_InterruptGate, 0x08); // IRQ6
    SetIDTGate((void*)IRQ7_handler, 0x27, IDT_TA_InterruptGate, 0x08); // IRQ7
    SetIDTGate((void*)IRQ8_handler, 0x28, IDT_TA_InterruptGate, 0x08); // IRQ8
    SetIDTGate((void*)IRQ9_handler, 0x29, IDT_TA_InterruptGate, 0x08); // IRQ9
    SetIDTGate((void*)IRQ10_handler, 0x2A, IDT_TA_InterruptGate, 0x08); // IRQ10
    SetIDTGate((void*)IRQ11_handler, 0x2B, IDT_TA_InterruptGate, 0x08); // IRQ11
    //SetIDTGate((void*)IRQ12_handler, 0x2C, IDT_TA_InterruptGate, 0x08); // IRQ12 Handled
    //SetIDTGate((void*)IRQ13_handler, 0x2D, IDT_TA_InterruptGate, 0x08); // IRQ13 Handled
    SetIDTGate((void*)IRQ14_handler, 0x2E, IDT_TA_InterruptGate, 0x08); // IRQ14
    SetIDTGate((void*)IRQ15_handler, 0x2F, IDT_TA_InterruptGate, 0x08); // IRQ15

     SetIDTGate((void*)intr_stub_49, 0x31, IDT_TA_InterruptGate | IDT_FLAG_RING3, 0x08); // SYSCALL

    RemapPIC(
        0xFF, //0b11111000, 
        0xFF //0b11101111
    );

    io_wait();    
    __asm__ volatile ("lidt %0" : : "m" (idtr));
    io_wait();    
    __asm__ volatile ("cli");
    io_wait();    

    //asm ("int $0x1");
}

void PrepareInterrupts2()
{
    AddToStack();
    // RemapPIC(
    //     0b11111000, //0b11111000, 
    //     0b11101111 //0b11101111
    // );
    RemapPIC(
        0, //0b11111000, 
        0 //0b11101111
    );
    io_wait();    
    __asm__ volatile ("sti");
    RemoveFromStack();
}

#include "../../devices/acpi/acpiShutdown.h"
#include "../../interrupts/panic.h"

void DoPCIWithoutMCFG(BootInfo* bootInfo)
{

}

void PrepareACPI(BootInfo* bootInfo)
{
    AddToStack();

    

    AddToStack();
    PrintMsgStartLayer("ACPI");
    PrintMsg("Preparing ACPI...");
    PrintfMsg("RSDP Addr: %X", (uint64_t)bootInfo->rsdp);
    RemoveFromStack();

    

    AddToStack();   
    ACPI::SDTHeader* rootThing = NULL;
    int div = 1;

    if (bootInfo->rsdp->firstPart.Revision == 0)
    {
        PrintMsg("ACPI Version: 1");
        rootThing = (ACPI::SDTHeader*)(uint64_t)(bootInfo->rsdp->firstPart.RSDTAddress);
        PrintfMsg("RSDT Header Addr: %X", (uint64_t)rootThing);
        div = 4;

        if (rootThing == NULL)
        {
            Panic("RSDT Header is at NULL!", true);
        }
        else
        {
            //GlobalRenderer->Clear(Colors.black);
            PrintMsg("> Testing ACPI Loader...");

            InitAcpiShutdownThing(rootThing);
            //while (true);
        }
    }
    else
    {
        PrintMsg("ACPI Version: 2");
        rootThing = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);

        PrintfMsg("XSDT Header Addr: %X", (uint64_t)rootThing);
        div = 8;

        if (rootThing == NULL)
        {
            Panic("XSDT Header is at NULL!", true);
        }
    }
    
    PrintDebugTerminal();
    RemoveFromStack();

    
    AddToStack();
    int entries = (rootThing->Length - sizeof(ACPI::SDTHeader)) / div;
    PrintfMsg("Entry count: %d", entries);
    RemoveFromStack();

    AddToStack();
    PrintMsgSL("> ");
    for (int t = 0; t < entries; t++)
    {
        uint64_t bleh1 = *(uint64_t*)((uint64_t)rootThing + sizeof(ACPI::SDTHeader) + (t * div));
        if (div == 4)
            bleh1 &= 0x00000000FFFFFFFF;
        ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)bleh1;
        
        char bruh[2];
        bruh[1] = 0;

        for (int i = 0; i < 4; i++)
        {
            bruh[0] = newSDTHeader->Signature[i];
            PrintMsgSL(bruh);
        }

        PrintMsgSL(" ");
    }
    PrintMsg("");
    RemoveFromStack();

    AddToStack();
    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(rootThing, (char*)"MCFG", div);

    PrintfMsg("MCFG Header Addr: %X", (uint64_t)mcfg);
    RemoveFromStack();

    if (mcfg == NULL)
    {
        //Panic("MCFG HEADER IS NULL!", true);
        DoPCIWithoutMCFG(bootInfo);

        PrintMsgEndLayer("ACPI");
        RemoveFromStack();
        return;
    }


    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.purple);

    //Panic("bruh", true);

    PrintDebugTerminal();

    AddToStack();
    PCI::EnumeratePCI(mcfg);
    RemoveFromStack();

    PrintDebugTerminal();

    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.bblue);

    AddToStack();
    PrintfMsg("Drive Count: %d", osData.diskInterfaces.GetCount());
    RemoveFromStack();    
    
    // for (int i = 0; i < 20; i++)
    //     GlobalRenderer->Clear(Colors.orange);

    PrintDebugTerminal();
    RemoveFromStack();

    PrintMsgEndLayer("ACPI");
}