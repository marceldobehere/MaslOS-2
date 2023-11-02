#pragma once
#include "paging.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "../kernelStuff/memory/memory.h"
#include <stdint.h>


// flags
#define PT_Flag_Present 1
#define PT_Flag_ReadWrite 2
#define PT_Flag_UserSuper 4
#define PT_Flag_WriteThrough 8
#define PT_Flag_CacheDisabled 16
#define PT_Flag_Accessed 32
#define PT_Flag_LargerPages 64
#define PT_Flag_Custom0 128
#define PT_Flag_Custom1 256
#define PT_Flag_Custom2 512
#define PT_Flag_PAT 1024


// kernel heap starts here
#define MEM_AREA_KERNEL_HEAP_START 0x0000100000000000

// where elfs will be mapped into
#define MEM_AREA_ELF_MAP_OFFSET 0x0000200000000000

// where the program kernel stacks will be mapped into
#define MEM_AREA_TASK_KERNEL_STACK_OFFSET 0x0000300000000000

// where the program user stacks will be mapped into
#define MEM_AREA_TASK_USER_STACK_OFFSET 0x0000400000000000

// where the program page tables will be mapped into
#define MEM_AREA_TASK_PAGE_TABLE_OFFSET 0
//0x0000500000000000

// when a user program requests a page it will be allocated here
//#define MEM_AREA_USER_PROGRAM_REQUEST_START 0x0000600000000000

// when a user program requests a page it will be allocated here
//#define MEM_AREA_USER_PROGRAM_HEAP ((Heap::HeapManager*)0x0000600000000000)

// when the kernel needs to get the memory from the current program it will be loaded here
//#define MEM_AREA_KERNEL_USER_COMMON_AREA_START 0x0000200000000000


class PageTableManager
{
    public:
    PageTable* PML4;
    PageTableManager(PageTable* PML4Address);

    void* GetPhysicalAddressFromVirtualAddress(void* virtualAddress);
    void* GetVirtualAddressFromPhysicalAddress(void* physicalAddress);
    int IsVirtualAddressMapped(void* virtualAddress);

    void MapMemory(void* virtualMemory, void* physicalMemory, int flags);
    void MapMemories(void* virtualMemory, void* physicalMemory, int c, int flags);

    void MapMemory(void* virtualMemory, void* physicalMemory);
    void MapMemories(void* virtualMemory, void* physicalMemory, int c);
    void UnmapMemory(void* virtualMemory);
    void UnmapMemories(void* virtualMemory, int c);

    PageTable* CreatePageTableContext();

    void SwitchPageTable(PageTable* PML4Address);
    void FreePageTable(PageTable* PML4Address);

    void MakeEveryEntryUserReadable();
    void PrintPageTable();
    void TrimPageTable();
};

void CopyPageTable(PageTable* srcPML4Address, PageTable* destPML4Address);

extern PageTableManager GlobalPageTableManager;