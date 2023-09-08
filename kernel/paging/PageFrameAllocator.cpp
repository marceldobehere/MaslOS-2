#include "PageFrameAllocator.h"
#include <stddef.h>
#include "../kernelStuff/memory/efiMemory.h"
#include "bitmap.h"
#include "../kernelStuff/memory/memory.h"
#include <libm/cstr.h>
#include "../interrupts/panic.h"
#include "../memory/heap.h"

PageFrameAllocator* GlobalAllocator;

void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress)
{
    PageBitMap.Size = bitmapSize;
    PageBitMap.Buffer = (uint8_t*) bufferAddress;
    for (int64_t i = 0; i < bitmapSize; i++)
        PageBitMap.Buffer[i] = 0;
    pageBitmapIndex = 0;
}

void PageFrameAllocator::ReservePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitMap[index])
        return;
    PageBitMap.Set(index, true);
    freeMemory -= 4096;
    reservedMemory += 4096;
}

void PageFrameAllocator::UnreservePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (!PageBitMap[index])
        return;
    if(!PageBitMap.Set(index, false))
        return;
    if (pageBitmapIndex > index)
        pageBitmapIndex = index;
    freeMemory += 4096;
    reservedMemory -= 4096;
}

void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        ReservePage((void*)((uint64_t)address + (i * 4096)));
}

void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        UnreservePage((void*)((uint64_t)address + (i * 4096)));
}


uint64_t PageFrameAllocator::GetFreeRAM()
{
    return freeMemory;
}
uint64_t PageFrameAllocator::GetUsedRAM()
{
    return usedMemory;
}
uint64_t PageFrameAllocator::GetReservedRAM()
{
    return reservedMemory;
}

int reqCount = 0;

void* PageFrameAllocator::RequestPage()
{
    reqCount++;
    for (; pageBitmapIndex < PageBitMap.Size * 8; pageBitmapIndex++)
    {
        if (PageBitMap[pageBitmapIndex])
            continue;
        LockPage((void*)(pageBitmapIndex * 4096));
        return(void*)(pageBitmapIndex * 4096);
    }
    
    for (pageBitmapIndex = 0; pageBitmapIndex < PageBitMap.Size * 8; pageBitmapIndex++)
    {
        if (PageBitMap[pageBitmapIndex])
            continue;
        LockPage((void*)(pageBitmapIndex * 4096));
        return(void*)(pageBitmapIndex * 4096);
    }

    
    GlobalRenderer->Println("ERROR: NO MORE RAM AVAIABLE!", Colors.red);
    GlobalRenderer->Println("REQ COUNT: {}", to_string(reqCount), Colors.red);
    GlobalRenderer->Println("FREE MEM: {}", to_string(freeMemory), Colors.yellow);
    GlobalRenderer->Println("USED MEM: {}", to_string(usedMemory), Colors.yellow);
    GlobalRenderer->Println("RES MEM: {}", to_string(reservedMemory), Colors.yellow);
    
    //SwitchToBackupHeap();
    Panic("No more RAM avaiable! (Count: {})", to_string(reqCount), true);


    return NULL; // Page Frame Swap to file
}

void PageFrameAllocator::FreePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (!PageBitMap[index])
        Panic("Cant free already free page!", true);//return;
    if(!PageBitMap.Set(index, false))
        Panic("Freeing page failed!", true);//return;
    if (pageBitmapIndex > index)
        pageBitmapIndex = index;
    freeMemory += 4096;
    usedMemory -= 4096;
}


void* PageFrameAllocator::RequestPages(int count)
{
    reqCount += count;
    int tCount = count;
    for (; pageBitmapIndex < PageBitMap.Size * 8; pageBitmapIndex++)
    {
        if (PageBitMap[pageBitmapIndex])
        {
            tCount = count;
            continue;
        }
        {
            tCount--;
            if (tCount <= 0)
            {
                LockPages((void*)((pageBitmapIndex - (count - 1)) * 4096), count);
                return(void*)((pageBitmapIndex - (count - 1)) * 4096);
            }
        }
    }
    
    tCount = count;
    for (pageBitmapIndex = 0; pageBitmapIndex < PageBitMap.Size * 8; pageBitmapIndex++)
    {
        if (PageBitMap[pageBitmapIndex])
        {
            tCount = count;
            continue;
        }
        {
            tCount--;
            if (tCount <= 0)
            {
                LockPages((void*)((pageBitmapIndex - (count - 1)) * 4096), count);
                return(void*)((pageBitmapIndex - (count - 1)) * 4096);
            }
        }
    }

    GlobalRenderer->Println("ERROR: NO MORE RAM AVAIABLE!", Colors.red);
    GlobalRenderer->Println("REQ COUNT: {}", to_string(reqCount), Colors.red);
    GlobalRenderer->Println("FREE MEM: {}", to_string(freeMemory), Colors.yellow);
    GlobalRenderer->Println("USED MEM: {}", to_string(usedMemory), Colors.yellow);
    GlobalRenderer->Println("RES MEM: {}", to_string(reservedMemory), Colors.yellow);
    
    //SwitchToBackupHeap();
    Panic("No more RAM avaiable! (Count: {})", to_string(reqCount), true);
    return NULL;
}

void PageFrameAllocator::FreePages(void* address, int count)
{
    for (int i = 0; i < count; i++)
        FreePage((void*)((uint64_t)address + (i * 4096)));
}

void PageFrameAllocator::LockPage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitMap[index])
        Panic("Cant lock already locked page!", true);//return;
    if(!PageBitMap.Set(index, true))
        Panic("Locking page failed!", true);//return;
    freeMemory -= 4096;
    usedMemory += 4096;
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        FreePage((void*)((uint64_t)address + (i * 4096)));
    
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount)
{
    for (int i = 0; i < pageCount; i++)
        LockPage((void*)((uint64_t)address + (i * 4096)));
}

void PageFrameAllocator::ReadEFIMemoryMap(void* start, uint64_t size)
{
    if (Initialized)
        return;
    Initialized = true;

    size -= 0x4000;

    void* largestFreeMemSeg = start;
    size_t largestFreeMemSegSize = size;
    uint64_t memStart = (uint64_t)largestFreeMemSeg;


    uint64_t memorySize = size;
    freeMemory = memorySize;
    reservedMemory = 0;
    usedMemory = 0;
    uint64_t bitmapSize =  ((memorySize / 0x1000) / 8) + 1;

    PrintMsgStartLayer("Info");
    PrintMsgCol("Largest Mem Size: {} Bytes.", to_string(largestFreeMemSegSize), Colors.yellow);
    PrintMsgCol("Larget Mem Loc: 0x{}", ConvertHexToString(memStart), Colors.yellow);
    PrintMsgCol("Bitmap Size:      {} Bytes.", to_string(bitmapSize + sizeof(Bitmap)), Colors.yellow);
    PrintMsgEndLayer("Info");

    PrintMsg("> Initing Bitmap");
    InitBitmap(bitmapSize, largestFreeMemSeg);

    
    int resPageCount = (memStart + 0xFFF) / 0x1000;
    PrintMsg("> Reserving first {} Pages", to_string(resPageCount));
    ReservePages(0, resPageCount);
    
    PrintMsg("> Reserving Pages for the Bitmap buffer starting at 0x{}", ConvertHexToString((uint64_t)PageBitMap.Buffer));
    ReservePages(PageBitMap.Buffer, bitmapSize / 0x1000 + 1);


    PrintMsgStartLayer("Info");
    PrintMsgCol("Bitmap ADDR:      {}", ConvertHexToString((uint64_t)PageBitMap.Buffer), Colors.yellow);
    //reservedMemory = data;
    PrintMsgCol("FREE MEM: {}", to_string(freeMemory), Colors.yellow);
    PrintMsgCol("USED MEM: {}", to_string(usedMemory), Colors.yellow);
    PrintMsgCol("RES MEM:  {}", to_string(reservedMemory), Colors.yellow);
    //GlobalRenderer->Println();
    PrintMsgEndLayer("Info");

    //while(true);
}

uint64_t PageFrameAllocator::GetFreePageCount()
{
    //GlobalRenderer->Println("Size: {}", to_string(PageBitMap.Size * 8), Colors.yellow);
    //GlobalRenderer->Print("Page: 0", Colors.yellow);
    uint64_t count = 0;
    for (uint64_t index = 0; index < PageBitMap.Size * 8; index++)
    {
        if (!PageBitMap[index])
            count++;
        //GlobalRenderer->CursorPosition.x = 0;
        
        //GlobalRenderer->Print("Page: {}    ", to_string(index), Colors.black);
        //GlobalRenderer->CursorPosition.x = 0;
        //GlobalRenderer->Print("Page: {}    ", to_string(index+1), Colors.silver);
    }

    //GlobalRenderer->CursorPosition.x = 0;
    //GlobalRenderer->Print("Page: {}    ", to_string(PageBitMap.Size * 8), Colors.black);
    //GlobalRenderer->CursorPosition.x = 0;

    //GlobalRenderer->Println();

    return count;
}