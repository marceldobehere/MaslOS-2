#include "memory.h"
#include <libm/cstr.h>

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize)
{
    uint64_t size = 0;
    for (int i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        size += desc->numPages * 4096;
    }

    return size;
}


void PrintEFIMemData(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize, BasicRenderer* temp )
{
    {
        temp->Println("EFI Memory Data:");

        

        temp->Print("There are "); 
        temp->Print(to_string(mMapEntries)); 
        temp->Println(" Entries: "); 


        for (int i = 0; i < mMapEntries; i++)
        {
            EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
            temp->Print(EFI_MEMORY_TYPE_STRINGS[desc->type]);
            temp->Print(" - "); 
            temp->color = Colors.cyan;
            temp->Print(to_string(desc->numPages * 4096 / 1024)); 
            temp->Print(" KB"); 
            temp->color = Colors.white;
            temp->Println("."); 
        }


    }
}


// void _memset(void* start, uint8_t value, uint64_t num)
// {
//     uint8_t* curr = (uint8_t*)start;
    
//     for (int64_t rem = num; rem > 0; rem--)
//         *curr++ = value;
// }

// void _memcpy(const void* src, void* dest, uint64_t size)
// {
//     const char* _src  = (const char*)src;
//     char* _dest = (char*)dest;
//     while (size--)
//     {
//         *_dest = *_src;
//         _dest++;
//         _src++;
//     }
// }

int _memcmp(const void* src, const void* dest, int amt)
{
    const char* fi = (const char*)src;
    const char* la = (const char*)dest;
    for (int i = 0; i < amt; i++)
    {
        if (fi[i] > la[i])
            return 1;
        if (fi[i] < la[i])
            return -1;
    }
    return 0;
}