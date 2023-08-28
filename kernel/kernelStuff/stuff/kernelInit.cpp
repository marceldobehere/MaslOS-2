#include "kernelInit.h"
#include "../../paging/PageTableManager.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include "../../devices/serial/serial.h"


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


    PrintMsgEndLayer("BOOT");

    StepDone();
    // if (PIT::TicksSinceBoot != 0)
    //     PIT::Sleep(200);

    Println();
    PrintMsgCol("> Inited Kernel!", Colors.bgreen);
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

    
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize;
    //GlobalAllocator->LockPages((void*)earlyVirtualToPhysicalAddr((void*)fbBase), fbSize / 0x1000);

    PrintMsg("> Mapping Framebuffer Memory");

    //for (uint64_t i = fbBase; i < fbBase + fbSize; i+=4096)
    //    GlobalPageTableManager.MapFramebufferMemory((void*)i, (void*)i);
}