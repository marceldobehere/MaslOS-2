#include "PageTableManager.h"
#include "../devices/serial/serial.h"



PageTableManager::PageTableManager(PageTable* PML4Address)
{
    this->PML4 = PML4Address;
}

void* PageTableManager::GetPhysicalAddressFromVirtualAddress(void* virtualAddress)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualAddress);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    if (!PDE.GetFlag(PT_Flag::Present))
        return NULL;

    PageTable* PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    PDE = PDP->entries[indexer.PD_i];
    if (!PDE.GetFlag(PT_Flag::Present))
        return NULL;

    PageTable* PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    PDE = PD->entries[indexer.PT_i];
    if (!PDE.GetFlag(PT_Flag::Present))
        return NULL;
    
    PageTable* PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    PDE = PT->entries[indexer.P_i];
    if (!PDE.GetFlag(PT_Flag::Present))
        return NULL;

    return (void*)((uint64_t)PDE.GetAddress() << 12);
}

void* PageTableManager::GetVirtualAddressFromPhysicalAddress(void* physicalAddress)
{
    for (uint64_t PML4_i = 0; PML4_i < 512; PML4_i++)
    {
        PageDirectoryEntry PDE = PML4->entries[PML4_i];
        if (!PDE.GetFlag(PT_Flag::Present))
            continue;

        PageTable* PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
        for (uint64_t PDP_i = 0; PDP_i < 512; PDP_i++)
        {
            PDE = PDP->entries[PDP_i];
            if (!PDE.GetFlag(PT_Flag::Present))
                continue;

            PageTable* PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
            for (uint64_t PD_i = 0; PD_i < 512; PD_i++)
            {
                PDE = PD->entries[PD_i];
                if (!PDE.GetFlag(PT_Flag::Present))
                    continue;

                PageTable* PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
                for (uint64_t PT_i = 0; PT_i < 512; PT_i++)
                {
                    PDE = PT->entries[PT_i];
                    if (!PDE.GetFlag(PT_Flag::Present))
                        continue;

                    if ((void*)((uint64_t)PDE.GetAddress() << 12) == physicalAddress)
                    {
                        return (void*)((PML4_i << 39) | (PDP_i << 30) | (PD_i << 21) | (PT_i << 12));
                    }
                }
            }
        }
    }

    return NULL;
}

int PageTableManager::IsVirtualAddressMapped(void* virtualAddress)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualAddress);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    if (!PDE.GetFlag(PT_Flag::Present))
        return 1;

    PageTable* PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    PDE = PDP->entries[indexer.PD_i];
    if (!PDE.GetFlag(PT_Flag::Present))
        return 2;

    PageTable* PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    PDE = PD->entries[indexer.PT_i];
    if (PDE.GetFlag(PT_Flag::Custom1))
        return 5;
    if (!PDE.GetFlag(PT_Flag::Present))
        return 3;
    
    PageTable* PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    PDE = PT->entries[indexer.P_i];    
    if (!PDE.GetFlag(PT_Flag::Present))
        return 4;

    return 0;
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
            PDE.SetFlag(PT_Flag::UserSuper, true);
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
            PDE.SetFlag(PT_Flag::UserSuper, true);
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
            PDE.SetFlag(PT_Flag::UserSuper, true);
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
    MapMemory((void*)((uint64_t)table + MEM_AREA_TASK_PAGE_TABLE_OFFSET), table, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
    table = (PageTable*)((uint64_t)table + MEM_AREA_TASK_PAGE_TABLE_OFFSET);
    
    _memset(table, 0, 0x1000);

    return table;
}

void PageTableManager::SwitchPageTable(PageTable* table)
{
    if ((uint64_t)table > MEM_AREA_TASK_PAGE_TABLE_OFFSET)
        table = (PageTable*)((uint64_t)table - MEM_AREA_TASK_PAGE_TABLE_OFFSET);

    asm volatile("mov %0, %%cr3" : : "r"((uint64_t)table) : "memory");
}

#include "../osData/MStack/MStackM.h"

void PageTableManager::FreePageTable(PageTable* table)
{
    AddToStack();
    table = (PageTable*)((uint64_t)table - MEM_AREA_TASK_PAGE_TABLE_OFFSET);
    
    // // go through every entry and free the pages
    // for (int i = 0; i < 512; i++)
    // {
    //     AddToStack();
    //     if (table->entries[i].GetFlag(PT_Flag::Present))
    //     {
    //         PageTable* PDP = (PageTable*)((uint64_t)table->entries[i].GetAddress() << 12);
    //         for (int j = 0; j < 512; j++)
    //         {
    //             AddToStack();
    //             if (PDP->entries[j].GetFlag(PT_Flag::Present))
    //             {
    //                 PageTable* PD = (PageTable*)((uint64_t)PDP->entries[j].GetAddress() << 12);
    //                 for (int k = 0; k < 512; k++)
    //                 {
    //                     AddToStack();
    //                     if (PD->entries[k].GetFlag(PT_Flag::Present))
    //                     {
    //                         PageTable* PT = (PageTable*)((uint64_t)PD->entries[k].GetAddress() << 12);
    //                         // for (int l = 0; l < 512; l++)
    //                         // {
    //                         //     if (PT->entries[l].GetFlag(PT_Flag::Present))
    //                         //     {
    //                         //         //GlobalAllocator->FreePage((void*)((uint64_t)PT->entries[l].GetAddress() << 12));
    //                         //     }
    //                         // }
    //                         GlobalAllocator->FreePage((void*)PT);
    //                     }
    //                     RemoveFromStack();
    //                 }
    //                 GlobalAllocator->FreePage((void*)PD);
    //             }
    //             RemoveFromStack();
    //         }
    //         GlobalAllocator->FreePage((void*)PDP);
    //     }
    //     RemoveFromStack();
    // }
     
    AddToStack();
    GlobalAllocator->FreePage((void*)table);
    RemoveFromStack();
    //UnmapMemory(table);

    RemoveFromStack();
}

void CopyPageTable(PageTable* srcPML4Address, PageTable* destPML4Address)
{
    // // do a deep copy not just a top level shallow copy
    // for (int i = 0; i < 512; i++)
    // {
    //     if (srcPML4Address->entries[i].GetFlag(PT_Flag::Present))
    //     {
    //         PageTable* srcPDP = (PageTable*)((uint64_t)srcPML4Address->entries[i].GetAddress() << 12);
    //         PageTable* destPDP = (PageTable*)GlobalAllocator->RequestPage();
    //         //GlobalPageTableManager.MapMemory(destPDP, destPDP, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
    //         _memset(destPDP, 0, 0x1000);
    //         destPML4Address->entries[i].SetAddress((uint64_t)destPDP >> 12);
    //         destPML4Address->entries[i].SetFlag(PT_Flag::Present, srcPML4Address->entries[i].GetFlag(PT_Flag::Present));
    //         destPML4Address->entries[i].SetFlag(PT_Flag::ReadWrite, srcPML4Address->entries[i].GetFlag(PT_Flag::ReadWrite));
    //         destPML4Address->entries[i].SetFlag(PT_Flag::CacheDisabled, srcPML4Address->entries[i].GetFlag(PT_Flag::CacheDisabled));
    //         destPML4Address->entries[i].SetFlag(PT_Flag::WriteThrough, srcPML4Address->entries[i].GetFlag(PT_Flag::WriteThrough));
    //         destPML4Address->entries[i].SetFlag(PT_Flag::UserSuper, srcPML4Address->entries[i].GetFlag(PT_Flag::UserSuper));
    //         for (int j = 0; j < 512; j++)
    //         {
    //             if (srcPDP->entries[j].GetFlag(PT_Flag::Present))
    //             {
    //                 PageTable* srcPD = (PageTable*)((uint64_t)srcPDP->entries[j].GetAddress() << 12);
    //                 PageTable* destPD = (PageTable*)GlobalAllocator->RequestPage();
    //                 //GlobalPageTableManager.MapMemory(destPD, destPD, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
    //                 _memset(destPD, 0, 0x1000);
    //                 destPDP->entries[j].SetAddress((uint64_t)destPD >> 12);
    //                 destPDP->entries[j].SetFlag(PT_Flag::Present, srcPDP->entries[j].GetFlag(PT_Flag::Present));
    //                 destPDP->entries[j].SetFlag(PT_Flag::ReadWrite, srcPDP->entries[j].GetFlag(PT_Flag::ReadWrite));
    //                 destPDP->entries[j].SetFlag(PT_Flag::CacheDisabled, srcPDP->entries[j].GetFlag(PT_Flag::CacheDisabled));
    //                 destPDP->entries[j].SetFlag(PT_Flag::WriteThrough, srcPDP->entries[j].GetFlag(PT_Flag::WriteThrough));
    //                 destPDP->entries[j].SetFlag(PT_Flag::UserSuper, srcPDP->entries[j].GetFlag(PT_Flag::UserSuper));

    //                 for (int k = 0; k < 512; k++)
    //                 {
    //                     if (srcPD->entries[k].GetFlag(PT_Flag::Present))
    //                     {
    //                         PageTable* srcPT = (PageTable*)((uint64_t)srcPD->entries[k].GetAddress() << 12);
    //                         PageTable* destPT = (PageTable*)GlobalAllocator->RequestPage();
    //                         //GlobalPageTableManager.MapMemory(destPT, destPT, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
    //                         _memset(destPT, 0, 0x1000);
    //                         destPD->entries[k].SetAddress((uint64_t)destPT >> 12);
    //                         destPD->entries[k].SetFlag(PT_Flag::Present, srcPD->entries[k].GetFlag(PT_Flag::Present));
    //                         destPD->entries[k].SetFlag(PT_Flag::ReadWrite, srcPD->entries[k].GetFlag(PT_Flag::ReadWrite));
    //                         destPD->entries[k].SetFlag(PT_Flag::CacheDisabled, srcPD->entries[k].GetFlag(PT_Flag::CacheDisabled));
    //                         destPD->entries[k].SetFlag(PT_Flag::WriteThrough, srcPD->entries[k].GetFlag(PT_Flag::WriteThrough));
    //                         destPD->entries[k].SetFlag(PT_Flag::UserSuper, srcPD->entries[k].GetFlag(PT_Flag::UserSuper));

    //                         for (int l = 0; l < 512; l++)
    //                         {
    //                             if (srcPT->entries[l].GetFlag(PT_Flag::Present))
    //                             {
    //                                 destPT->entries[l].SetAddress(srcPT->entries[l].GetAddress());
    //                                 destPT->entries[l].SetFlag(PT_Flag::Present, srcPT->entries[l].GetFlag(PT_Flag::Present));
    //                                 destPT->entries[l].SetFlag(PT_Flag::ReadWrite, srcPT->entries[l].GetFlag(PT_Flag::ReadWrite));
    //                                 destPT->entries[l].SetFlag(PT_Flag::CacheDisabled, srcPT->entries[l].GetFlag(PT_Flag::CacheDisabled));
    //                                 destPT->entries[l].SetFlag(PT_Flag::WriteThrough, srcPT->entries[l].GetFlag(PT_Flag::WriteThrough));
    //                                 destPT->entries[l].SetFlag(PT_Flag::UserSuper, srcPT->entries[l].GetFlag(PT_Flag::UserSuper));
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }


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

void PageTableManager::PrintPageTable()
{
    Serial::Writelnf("PML4: %X", PML4);
    uint64_t virtualAddress;

    for (uint64_t i = 0; i < 512; i++)
    {
        if (PML4->entries[i].GetFlag(PT_Flag::Present))
        {
            virtualAddress = (i << 39) | (0 << 30) | (0 << 21) | (0 << 12);
            Serial::Writelnf("> PDP: %X -> %X", virtualAddress, (uint64_t)PML4->entries[i].GetAddress() << 12);
            PageTable* PDP = (PageTable*)((uint64_t)PML4->entries[i].GetAddress() << 12);
            for (uint64_t j = 0; j < 512; j++)
            {
                if (PDP->entries[j].GetFlag(PT_Flag::Present))
                {
                    virtualAddress = (i << 39) | (j << 30) | (0 << 21) | (0 << 12);
                    Serial::Writelnf("  > PD: %X -> %X", virtualAddress, (uint64_t)PDP->entries[j].GetAddress() << 12);
                    PageTable* PD = (PageTable*)((uint64_t)PDP->entries[j].GetAddress() << 12);
                    for (uint64_t k = 0; k < 512; k++)
                    {
                        if (PD->entries[k].GetFlag(PT_Flag::Present))
                        {
                            virtualAddress = (i << 39) | (j << 30) | (k << 21) | (0 << 12);
                            Serial::Writelnf("    > PT: %X -> %X", virtualAddress, (uint64_t)PD->entries[k].GetAddress() << 12);
                            PageTable* PT = (PageTable*)((uint64_t)PD->entries[k].GetAddress() << 12);
                            for (uint64_t l = 0; l < 512; l++)
                            {
                                if (PT->entries[l].GetFlag(PT_Flag::Present))
                                {
                                    virtualAddress = (i << 39) | (j << 30) | (k << 21) | (l << 12);
                                    Serial::Writelnf("      > P: %X -> %X", virtualAddress, (uint64_t)PT->entries[l].GetAddress() << 12);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void PageTableManager::TrimPageTable()
{
    uint64_t virtualAddress;

    // repeatFromTop:

    // for (uint64_t i = 0; i < 512; i++)
    // {
    //     if (PML4->entries[i].GetFlag(PT_Flag::Present))
    //     {
    //         virtualAddress = (i << 39) | (0 << 30) | (0 << 21) | (0 << 12);
    //         Serial::Writelnf("  > PDP: %X -> %X", virtualAddress, (uint64_t)PML4->entries[i].GetAddress() << 12);
    //         PageTable* PDP = (PageTable*)((uint64_t)PML4->entries[i].GetAddress() << 12);
    //         //int PD_COUNT = 0;
    //         for (uint64_t j = 0; j < 512; j++)
    //         {
    //             if (PDP->entries[j].GetFlag(PT_Flag::Present))
    //             {
    //                 //PD_COUNT++;
    //                 virtualAddress = (i << 39) | (j << 30) | (0 << 21) | (0 << 12);
    //                 Serial::Writelnf("    > PD: %X -> %X", virtualAddress, (uint64_t)PDP->entries[j].GetAddress() << 12);
    //                 PageTable* PD = (PageTable*)((uint64_t)PDP->entries[j].GetAddress() << 12);
    //                 // if (!IsVirtualAddressMapped(PD))
    //                 // {
    //                 //     Serial::Writelnf("      > UNMAPPED PD: %X -> %X", virtualAddress, (uint64_t)PDP->entries[j].GetAddress() << 12);
    //                 //     PDP->entries[j].SetFlag(PT_Flag::Present, 0);
    //                 //     continue;
    //                 // }
    //                 //int PT_COUNT = 0;
    //                 for (uint64_t k = 0; k < 512; k++)
    //                 {
    //                     if (PD->entries[k].GetFlag(PT_Flag::Present))
    //                     {
    //                         //PT_COUNT++;
    //                         virtualAddress = (i << 39) | (j << 30) | (k << 21) | (0 << 12);
    //                         Serial::Writelnf("      > PT: %X -> %X", virtualAddress, (uint64_t)PD->entries[k].GetAddress() << 12);
    //                         PageTable* PT = (PageTable*)((uint64_t)PD->entries[k].GetAddress() << 12);
    //                         int mapState = IsVirtualAddressMapped(PT);
    //                         if (mapState == 5)
    //                         {
    //                             Serial::Writelnf("      > UNMAPPED PT: %X (%d) -> %X", (uint64_t)PT, mapState, GetPhysicalAddressFromVirtualAddress(PT));
    //                             //PD->entries[k].SetFlag(PT_Flag::Present, 0);
    //                             continue;
    //                         }
    //                         int P_COUNT = 0;
    //                         for (uint64_t l = 0; l < 512; l++)
    //                         {
    //                             PageDirectoryEntry P = PT->entries[l];
    //                             if (P.GetFlag(PT_Flag::Present))
    //                             {
    //                                 virtualAddress = (i << 39) | (j << 30) | (k << 21) | (l << 12);
    //                                 uint64_t addr = P.GetAddress();
    //                                 if (addr == 0xFFFFFFFFFF || addr == 0)
    //                                 {
    //                                     //Serial::Writelnf("        > P: %X -> %X", virtualAddress, addr << 12);
    //                                     P_COUNT++;
    //                                 }
                                    
    //                                 //Serial::Writelnf("        > P: %X -> %X", virtualAddress, (uint64_t)PT->entries[l].GetAddress() << 12);
    //                             }
    //                             else if (P.GetAddress() != 0)
    //                             {
    //                                 virtualAddress = (i << 39) | (j << 30) | (k << 21) | (l << 12);
    //                                 //Serial::Writelnf("        < P: %X -> %X", virtualAddress, (uint64_t)PT->entries[l].GetAddress() << 12);
    //                             }
    //                         }

    //                         if (P_COUNT == 512)
    //                         {
    //                             virtualAddress = (i << 39) | (j << 30) | (k << 21) | (0 << 12);
    //                             Serial::Writelnf("    RM> PT: %X -> %X, (%d, %d, %d)", virtualAddress, (uint64_t)PD->entries[k].GetAddress() << 12, i, j, k);
    //                             PD->entries[k].SetFlag(PT_Flag::Custom1, 1);
    //                             PD->entries[k].SetFlag(PT_Flag::Present, 0);
    //                             //PD->entries[k] = PageDirectoryEntry();
    //                         }
    //                         else
    //                             PD->entries[k].SetFlag(PT_Flag::Custom1, 0);
    //                     }
    //                 }

    //                 // if (PT_COUNT == 0)
    //                 // {
    //                 //     // virtualAddress = (i << 39) | (j << 30) | (0 << 21) | (0 << 12);
    //                 //     // Serial::Writelnf("  RM> PD: %X -> %X", virtualAddress, (uint64_t)PDP->entries[j].GetAddress() << 12);
    //                 //     // PDP->entries[j].SetFlag(PT_Flag::Present, 0);
    //                 // }
    //             }
    //         }

    //         // if (PD_COUNT == 0)
    //         // {
    //         //     // virtualAddress = (i << 39) | (0 << 30) | (0 << 21) | (0 << 12);
    //         //     // Serial::Writelnf("RM> PDP: %X -> %X", virtualAddress, (uint64_t)PML4->entries[i].GetAddress() << 12);
    //         //     // PML4->entries[i].SetFlag(PT_Flag::Present, 0);
    //         // }
    //     }
    // }
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