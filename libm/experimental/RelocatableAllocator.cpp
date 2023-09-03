#include "RelocatableAllocator.h"

char *RelocatableAllocator::alloc(uint64_t size)
{
    int idx = FindFreeBlock(size);
    if (FreeBlocks[idx].Size > size)
    {
        char *ptr = FreeBlocks[idx].Pointer;
        FreeBlocks[idx].Size -= size;
        FreeBlocks[idx].Pointer += size;
        Blocks[NumberOfBlocks].Size = size;
        Blocks[NumberOfBlocks++].Pointer = ptr;
        return ptr;
    }
    else
    {
        char *ptr = FreeBlocks[idx].Pointer;
        for (int i = idx; i < NumberOfFreeBlocks - 1; ++i)
        {
            FreeBlocks[i] = FreeBlocks[i + 1];
        }
        --NumberOfFreeBlocks;
        Blocks[NumberOfBlocks].Size = size;
        Blocks[NumberOfBlocks++].Pointer = ptr;
        return ptr;
    }
    return nullptr;
}

void RelocatableAllocator::free(char *ptr)
{
    for (int i = 0; i < NumberOfBlocks; ++i)
    {
        if (Blocks[i].Pointer == ptr)
        {
            auto sz = Blocks[i].Size;
            for (int j = i; j < NumberOfBlocks - 1; ++j)
            {
                Blocks[j] = Blocks[j + 1];
            }
            FreeBlocks[NumberOfFreeBlocks].Pointer = ptr;
            FreeBlocks[NumberOfFreeBlocks++].Size = sz;
        }
    }
}

RelocatableAllocator::RelocatableAllocator(char *buffer, int totalSize)
{
    Size = totalSize;
    Buffer = buffer;
    FreeBlocks[0].Pointer = Buffer;
    FreeBlocks[0].Size = Size;
}
