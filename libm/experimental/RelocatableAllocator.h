#pragma once
#include <stdint.h>

struct RelocatableBlock
{
    char *Pointer;
    uint64_t Size;
};
class RelocatableAllocator
{
private:
    char *Buffer;
    RelocatableBlock Blocks[512];
    RelocatableBlock FreeBlocks[512];
    int NumberOfBlocks = 1;
    int NumberOfFreeBlocks = 1;
    int FindFreeBlock(uint64_t Size)
    {
        int Smallest = 0;
        for (int i = 0; i < NumberOfFreeBlocks; ++i)
        {
            if (FreeBlocks[i].Size >= Size && FreeBlocks[i].Size < FreeBlocks[Smallest].Size)
            {
                Smallest = i;
            }
        }
        return Smallest;
    }

public:
    int Size;
    RelocatableAllocator()
    {
        Buffer = nullptr;
        Size = 0;
    }
    RelocatableAllocator(char *buffer, int totalSize);
    char *alloc(uint64_t size);
    void free(char *ptr);
};