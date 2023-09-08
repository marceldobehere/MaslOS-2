#include "bitmap.h"
#include <stddef.h>
#include <stdint.h>
#include "../osData/MStack/MStackM.h"


size_t Size;
uint8_t* Buffer;

bool Bitmap::operator[](uint64_t index)
{
    AddToStack();
    if (index > Size * 8)
    {
        RemoveFromStack();
        return false;
    }

    uint64_t index2 = index >> 3;
    uint8_t bitIndexer = 0b10000000;
    bitIndexer = bitIndexer >> (index & 7);
    bool temp = (Buffer[index2] & bitIndexer) != 0;

    RemoveFromStack();
    return temp;
}

bool Bitmap::Set(uint64_t index, bool value)
{
    AddToStack();
    if (index > Size * 8)
    {
        RemoveFromStack();
        return false;
    }

    uint64_t index2 = index >> 3;
    uint8_t bitIndexer = 0b10000000;
    bitIndexer = bitIndexer >> (index & 7);
    Buffer[index2] &= (~bitIndexer);

    if (value)
        Buffer[index2] |= bitIndexer;
        
    RemoveFromStack();
    return true;
}