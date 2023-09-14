#include "memStuff.h"

#ifndef _KERNEL_SRC
void _memcpy(void* src, void* dest, uint64_t size)
{
    char* d = (char*)dest;
    char* s = (char*)src;

    for (uint64_t i = 0; i < size; i++)
       *(d++) = *(s++);
}

void _memset(void* dest, uint8_t value, uint64_t size)
{
    char* d = (char*)dest;

    for (uint64_t i = 0; i < size; i++)
        *(d++) = value;
}
#endif