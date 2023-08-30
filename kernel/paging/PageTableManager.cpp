#include "PageTableManager.h"
#include "../devices/serial/serial.h"



PageTableManager::PageTableManager(PageTable* PML4Address)
{
    this->PML4 = PML4Address;
}



void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory, int flags)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;
    bool userSpace = ((flags & PT_Flag_UserSuper) != 0);
    //Serial::Writelnf("%X -> %X, %d, %B", (uint64_t)virtualMemory, (uint64_t)physicalMemory, flags, userSpace);

    PDE = PML4->entries[indexer.PDP_i];
    PageTable* PDP;

    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PDP = (PageTable*)GlobalAllocator->RequestPage();
        _memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, (flags & PT_Flag_Present) != 0);
        PDE.SetFlag(PT_Flag::ReadWrite, (flags & PT_Flag_ReadWrite) != 0);
        PDE.SetFlag(PT_Flag::CacheDisabled, (flags & PT_Flag_CacheDisabled) != 0);
        PDE.SetFlag(PT_Flag::WriteThrough, (flags & PT_Flag_WriteThrough) != 0);
        PDE.SetFlag(PT_Flag::UserSuper, (flags & PT_Flag_UserSuper) != 0);

        PML4->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        if (userSpace && !PDE.GetFlag(PT_Flag::UserSuper))
        {
            PDE.SetFlag(PT_Flag::UserSuper, 1);
            PML4->entries[indexer.PDP_i] = PDE;
        }
    }



    PDE = PDP->entries[indexer.PD_i];
    PageTable* PD;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PD = (PageTable*)GlobalAllocator->RequestPage();
        _memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, (flags & PT_Flag_Present) != 0);
        PDE.SetFlag(PT_Flag::ReadWrite, (flags & PT_Flag_ReadWrite) != 0);
        PDE.SetFlag(PT_Flag::CacheDisabled, (flags & PT_Flag_CacheDisabled) != 0);
        PDE.SetFlag(PT_Flag::WriteThrough, (flags & PT_Flag_WriteThrough) != 0);
        PDE.SetFlag(PT_Flag::UserSuper, (flags & PT_Flag_UserSuper) != 0);
        PDP->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        if (userSpace && !PDE.GetFlag(PT_Flag::UserSuper))
        {
            PDE.SetFlag(PT_Flag::UserSuper, 1);
            PDP->entries[indexer.PD_i] = PDE;
        }
    }




    PDE = PD->entries[indexer.PT_i];
    PageTable* PT;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PT = (PageTable*)GlobalAllocator->RequestPage();
        _memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, (flags & PT_Flag_Present) != 0);
        PDE.SetFlag(PT_Flag::ReadWrite, (flags & PT_Flag_ReadWrite) != 0);
        PDE.SetFlag(PT_Flag::CacheDisabled, (flags & PT_Flag_CacheDisabled) != 0);
        PDE.SetFlag(PT_Flag::WriteThrough, (flags & PT_Flag_WriteThrough) != 0);
        PDE.SetFlag(PT_Flag::UserSuper, (flags & PT_Flag_UserSuper) != 0);
        PD->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        if (userSpace && !PDE.GetFlag(PT_Flag::UserSuper))
        {
            PDE.SetFlag(PT_Flag::UserSuper, 1);
            PD->entries[indexer.PT_i] = PDE;
        }
    }


    
    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, (flags & PT_Flag_Present) != 0);
    PDE.SetFlag(PT_Flag::ReadWrite, (flags & PT_Flag_ReadWrite) != 0);
    PDE.SetFlag(PT_Flag::CacheDisabled, (flags & PT_Flag_CacheDisabled) != 0);
    PDE.SetFlag(PT_Flag::WriteThrough, (flags & PT_Flag_WriteThrough) != 0);
    PDE.SetFlag(PT_Flag::UserSuper, (flags & PT_Flag_UserSuper) != 0);
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
    MapMemory(table, table, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
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

void CopyPageTable(PageTable* srcPML4Address, PageTable* destPML4Address)
{
    for (int i = 0; i < 512; i++)
    {
        if (srcPML4Address->entries[i].GetFlag(PT_Flag::Present))
        {
            destPML4Address->entries[i] = srcPML4Address->entries[i];
            // void* src = (void*)((uint64_t)srcPML4Address->entries[i].GetAddress() << 12);
            // void* dest = (void*)((uint64_t)destPML4Address->entries[i].GetAddress() << 12);
            // if (dest == NULL)
            // {
            //     dest = (void*)GlobalAllocator->RequestPage();
            //     GlobalPageTableManager.MapMemory(dest, dest);
            //     destPML4Address->entries[i].SetAddress((uint64_t)dest >> 12);
            //     destPML4Address->entries[i].SetFlag(PT_Flag::Present, srcPML4Address->entries[i].GetFlag(PT_Flag::Present));
            //     destPML4Address->entries[i].SetFlag(PT_Flag::ReadWrite, srcPML4Address->entries[i].GetFlag(PT_Flag::ReadWrite));
            //     destPML4Address->entries[i].SetFlag(PT_Flag::CacheDisabled, srcPML4Address->entries[i].GetFlag(PT_Flag::CacheDisabled));
            //     destPML4Address->entries[i].SetFlag(PT_Flag::WriteThrough, srcPML4Address->entries[i].GetFlag(PT_Flag::WriteThrough));
            //     destPML4Address->entries[i].SetFlag(PT_Flag::UserSuper, srcPML4Address->entries[i].GetFlag(PT_Flag::UserSuper));
            // }
            // _memcpy(src, dest, 0x1000);
        }
    }
    
}

void PageTableManager::MakeEveryEntryUserReadable()
{
    // for (int i = 0; i < 512; i++)
    // {
    //     if (PML4->entries[i].GetFlag(PT_Flag::Present))
    //     {
    //         PML4->entries[i].SetFlag(PT_Flag::UserSuper, 1);
    //         //PML4->entries[i].SetFlag(PT_Flag::ReadWrite, 1);
    //         PageTable* PDP = (PageTable*)((uint64_t)PML4->entries[i].GetAddress() << 12);
    //         for (int j = 0; j < 512; j++)
    //         {
    //             if (PDP->entries[j].GetFlag(PT_Flag::Present))
    //             {
    //                 PDP->entries[j].SetFlag(PT_Flag::UserSuper, 1);
    //                 //PDP->entries[j].SetFlag(PT_Flag::ReadWrite, 1);
    //                 PageTable* PD = (PageTable*)((uint64_t)PDP->entries[j].GetAddress() << 12);
    //                 for (int k = 0; k < 512; k++)
    //                 {
    //                     if (PD->entries[k].GetFlag(PT_Flag::Present))
    //                     {
    //                         PD->entries[k].SetFlag(PT_Flag::UserSuper, 1);
    //                         //PD->entries[k].SetFlag(PT_Flag::ReadWrite, 1);
    //                         PageTable* PT = (PageTable*)((uint64_t)PD->entries[k].GetAddress() << 12);
    //                         for (int l = 0; l < 512; l++)
    //                         {
    //                             if (PT->entries[l].GetFlag(PT_Flag::Present))
    //                             {
    //                                 PT->entries[l].SetFlag(PT_Flag::UserSuper, 0);
    //                                 //PT->entries[l].SetFlag(PT_Flag::ReadWrite, 1);
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}

PageTableManager GlobalPageTableManager = NULL;