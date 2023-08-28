#include "PageTableManager.h"



PageTableManager::PageTableManager(PageTable* PML4Address)
{
    this->PML4 = PML4Address;
}



void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory, int flags)
{
    //return;
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable* PDP;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PDP = (PageTable*)GlobalAllocator->RequestPage();
        _memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, flags & PT_Flag_Present != 0);
        PDE.SetFlag(PT_Flag::ReadWrite, flags & PT_Flag_ReadWrite != 0);
        PDE.SetFlag(PT_Flag::CacheDisabled, flags & PT_Flag_CacheDisabled != 0);
        PDE.SetFlag(PT_Flag::WriteThrough, flags & PT_Flag_WriteThrough != 0);
        PDE.SetFlag(PT_Flag::UserSuper, flags & PT_Flag_UserSuper != 0);

        PML4->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }


    PDE = PDP->entries[indexer.PD_i];
    PageTable* PD;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PD = (PageTable*)GlobalAllocator->RequestPage();
        _memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, flags & PT_Flag_Present != 0);
        PDE.SetFlag(PT_Flag::ReadWrite, flags & PT_Flag_ReadWrite != 0);
        PDE.SetFlag(PT_Flag::CacheDisabled, flags & PT_Flag_CacheDisabled != 0);
        PDE.SetFlag(PT_Flag::WriteThrough, flags & PT_Flag_WriteThrough != 0);
        PDP->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }


    PDE = PD->entries[indexer.PT_i];
    PageTable* PT;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PT = (PageTable*)GlobalAllocator->RequestPage();
        _memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, flags & PT_Flag_Present != 0);
        PDE.SetFlag(PT_Flag::ReadWrite, flags & PT_Flag_ReadWrite != 0);
        PDE.SetFlag(PT_Flag::CacheDisabled, flags & PT_Flag_CacheDisabled != 0);
        PDE.SetFlag(PT_Flag::WriteThrough, flags & PT_Flag_WriteThrough != 0);
        PD->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }


    
    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, flags & PT_Flag_Present != 0);
    PDE.SetFlag(PT_Flag::ReadWrite, flags & PT_Flag_ReadWrite != 0);
    PDE.SetFlag(PT_Flag::CacheDisabled, flags & PT_Flag_CacheDisabled != 0);
    PDE.SetFlag(PT_Flag::WriteThrough, flags & PT_Flag_WriteThrough != 0);
    PT->entries[indexer.P_i] = PDE;


    return;
}

void PageTableManager::MapMemories(void* virtualMemory, void* physicalMemory, int c, int flags)
{
    for (int i = 0; i < c; i++)
    {
        MapMemory((void*)((uint64_t)virtualMemory + (i * 0x1000)), (void*)((uint64_t)physicalMemory + (i * 0x1000)), flags);
    }
}



void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory)
{
    MapMemory(virtualMemory, physicalMemory, PT_Flag_Present | PT_Flag_ReadWrite);
}
void PageTableManager::MapMemories(void* virtualMemory, void* physicalMemory, int c)
{
    MapMemories(virtualMemory, physicalMemory, c, PT_Flag_Present | PT_Flag_ReadWrite);
}
void PageTableManager::UnmapMemory(void* virtualMemory)
{
    MapMemory(virtualMemory, (void*)NULL, 0);
}
void PageTableManager::UnmapMemories(void* virtualMemory, int c)
{
    MapMemories(virtualMemory, (void*)NULL, c, 0);
}




PageTable* PageTableManager::CreatePageTableContext()
{
    PageTable* table = (PageTable*)GlobalAllocator->RequestPage();
    MapMemory(table, table);
    _memset(table, 0, 0x1000);

    return table;
}

void PageTableManager::SwitchPageTable(PageTable* PML4Address)
{
    //this->PML4 = PML4Address;
    asm volatile("mov %0, %%cr3" : : "r"(&PML4Address->entries));
}

void PageTableManager::FreePageTable(PageTable* PML4Address)
{
    UnmapMemory(PML4Address);
}

void PageTableManager::CopyPageTable(PageTable* srcPML4Address, PageTable* destPML4Address)
{
    for (int i = 0; i < 512; i++)
    {
        if (srcPML4Address->entries[i].GetFlag(PT_Flag::Present))
        {
            void* src = (void*)((uint64_t)srcPML4Address->entries[i].GetAddress() << 12);
            void* dest = (void*)((uint64_t)destPML4Address->entries[i].GetAddress() << 12);
            if (dest == NULL)
            {
                dest = (void*)GlobalAllocator->RequestPage();
                GlobalPageTableManager.MapMemory(dest, dest);
                destPML4Address->entries[i].SetAddress((uint64_t)dest >> 12);
                destPML4Address->entries[i].SetFlag(PT_Flag::Present, 1);
            }
            _memcpy(src, dest, 0x1000);
        }
    }
}


PageTableManager GlobalPageTableManager = NULL;