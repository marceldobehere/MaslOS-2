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
    
    
    

    PrintMsg("> Initializing Heap");
    InitializeHeap((void*)0x0000100000000000, 0x10);
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
    PrintMsgColSL("TIME: ", Colors.yellow);
    PrintMsgColSL("{}:", to_string((int)RTC::Hour), Colors.yellow);
    PrintMsgColSL("{}:", to_string((int)RTC::Minute), Colors.yellow);
    PrintMsgCol("{}", to_string((int)RTC::Second), Colors.yellow);
    
    PrintMsgColSL("DATE: ", Colors.yellow);
    PrintMsgColSL("{}.", to_string((int)RTC::Day), Colors.yellow);
    PrintMsgColSL("{}.", to_string((int)RTC::Month), Colors.yellow);
    PrintMsgCol("{}", to_string((int)RTC::Year), Colors.yellow);
    PrintMsgEndLayer("RTC Info");

    PrintMsg("> Initing PIT");
    PIT::InitPIT();
    StepDone();

//     #define STAT 0x64
//     #define CMD 0x60
    
//     PrintMsg("> Clearing Input Buffer (1/2)");
//     {
//         // Clear the input buffer.
//         size_t timeout = 1024;
//         while ((inb(STAT) & 1) && timeout > 0) {
//             timeout--;
//             inb(CMD);
//         }
//     }
//     StepDone();
    
//     PrintMsg("> Initing PS/2 Mouse");
//     InitPS2Mouse(bootInfo->mouseZIP, "default.mbif");
//     //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));
//     StepDone();

//     PrintMsg("> Initing Keyboard State List");
//     InitKeyboardListRam();
//     StepDone();

//     PrintMsg("> Initing PS/2 Keyboard");
//     InitKeyboard();
//     StepDone();
    
    PrintMsg("> Initialising Scheduler");
    Scheduler::InitScheduler();
    StepDone();

    PrintMsg("> Preparing Interrupts");
    PrepareInterrupts();
    PIT::Inited = true;
    StepDone();


//     PrintMsg("> Clearing Input Buffer (2/2)");
//     {
//         // Clear the input buffer.
//         size_t timeout = 1024;
//         while ((inb(STAT) & 1) && timeout > 0) {
//             timeout--;
//             inb(CMD);
//         }
//     }
//     StepDone();










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

void DoGdtStuff()
{
    //GlobalRenderer->Clear(Colors.red);
    GDTBlock* gdt_block = (GDTBlock*)GlobalAllocator->RequestPage();
    //GlobalRenderer->Clear(Colors.green);
    GlobalPageTableManager.MapMemory(gdt_block, gdt_block);
    //GlobalRenderer->Clear(Colors.blue);
    Serial::Writelnf("GDT: %X", (uint64_t)gdt_block);
    

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

    gdt_init(gdt_block);
	gdt_set_tss_ring(gdt_block, 0, stack_kernel_end);
	gdt_set_tss_ist(gdt_block, IDT_IST_ISR, stack_isr_end);
	gdt_set_tss_ist(gdt_block, IDT_IST_IRQ, stack_irq_end);
	gdt_set_tss_ist(gdt_block, IDT_IST_TIMER, stack_timer_end);
	gdt_load(&gdt_block->gdt_descriptor);

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
    GlobalAllocator->ReadEFIMemoryMap(bootInfo->mMapStart, bootInfo->mMapSize); 
    PrintMsgEndLayer("EFI Memory Map");

    

    //uint64_t kernelSize = (((uint64_t)&_KernelEnd) - ((uint64_t)&_KernelStart));
    //uint64_t kernelPages = ((uint64_t)kernelSize / 4096) + 1;

    //GlobalAllocator->LockPages(&_KernelStart, kernelPages);

    uint64_t rFB = earlyVirtualToPhysicalAddr(GlobalRenderer->framebuffer->BaseAddress);

    PageTable* PML4 = (PageTable*)GlobalAllocator->RequestPage();
    //GlobalPageTableManager.MapMemory(PML4, PML4);
    _memset(PML4, 0, 0x1000);
    PrintMsg("> Getting PML4 Stuff");
    //GlobalRenderer->Println("PML4 ADDR:          {}", ConvertHexToString((uint64_t)PML4), Colors.yellow);
    asm volatile("mov %%cr3, %0" : "=r"(PML4));
    // asm("mov %0, %%cr3" : : "r" (PML4) );
    PrintMsgStartLayer("Info");
    PrintMsgCol("PML4 ADDR:          {}", ConvertHexToString((uint64_t)PML4), Colors.yellow);
    PrintMsgCol("FB 1 ADDR:          {}", ConvertHexToString(rFB), Colors.yellow);
    PrintMsgCol("FB 2 ADDR:          {}", ConvertHexToString((uint64_t)GlobalRenderer->framebuffer->BaseAddress), Colors.yellow);
    PrintMsgCol("FB 3 ADDR:          {}", ConvertHexToString((uint64_t)bootInfo->framebuffer->BaseAddress), Colors.yellow);
    PrintMsgCol("MMAP ADDR:          {}", ConvertHexToString((uint64_t)bootInfo->mMapStart), Colors.yellow);
    PrintMsgCol("KERNEL ADDR:        {}", ConvertHexToString((uint64_t)bootInfo->kernelStart), Colors.yellow);
    PrintMsgCol("KERNEL V ADDR:      {}", ConvertHexToString((uint64_t)bootInfo->kernelStartV), Colors.yellow);
    GlobalPageTableManager = PageTableManager(PML4);
    PrintMsgEndLayer("Info");

    GlobalPageTableManager.MakeEveryEntryUserReadable();

    
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize;
    //GlobalAllocator->LockPages((void*)earlyVirtualToPhysicalAddr((void*)fbBase), fbSize / 0x1000);

    PrintMsg("> Mapping Framebuffer Memory");

    //for (uint64_t i = fbBase; i < fbBase + fbSize; i+=4096)
    //    GlobalPageTableManager.MapFramebufferMemory((void*)i, (void*)i);
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


    io_wait();    
    __asm__ volatile ("lidt %0" : : "m" (idtr));
    io_wait();    
    //asm ("int $0x1");

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