#include "PageTableManager.h"



PageTableManager::PageTableManager(PageTable* PML4Address)
{
    this->PML4 = PML4Address;
}

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


PageTableManager GlobalPageTableManager = NULL;