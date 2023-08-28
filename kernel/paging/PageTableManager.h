#pragma once
#include "paging.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "../kernelStuff/memory/memory.h"
#include <stdint.h>


class PageTableManager
{
    public:
    PageTable* PML4;
    PageTableManager(PageTable* PML4Address);

    void MapMemory(void* virtualMemory, void* physicalMemory, int flags);
    void MapMemories(void* virtualMemory, void* physicalMemory, int c, int flags);

    void MapMemory(void* virtualMemory, void* physicalMemory);
    void MapMemories(void* virtualMemory, void* physicalMemory, int c);
    void UnmapMemory(void* virtualMemory);
    void UnmapMemories(void* virtualMemory, int c);

    PageTable* CreatePageTableContext();
    void SwitchPageTable(PageTable* PML4Address);
    void FreePageTable(PageTable* PML4Address);

};

extern PageTableManager GlobalPageTableManager;