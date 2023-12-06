#include "memStuff.h"

#ifndef _KERNEL_SRC
void _memcpy(void* src, void* dest, uint64_t size)
{
    const char* _src  = (const char*)src;
    char* _dest = (char*)dest;
    while (size--)
    {
        *_dest = *_src;
        _dest++;
        _src++;
    }
}

void _memset(void* dest, uint8_t value, uint64_t size)
{
    char* d = (char*)dest;

    for (uint64_t i = 0; i < size; i++)
        *(d++) = value;
}
#endif

void _memmove(void* src, void* dest, uint64_t size) {
	char* d = (char*) dest;
	char* s = (char*) src;
	if(d < s) {
		while(size--) {
			*d++ = *s++;
		}
	} else {
		d += size;
		s += size;
		while(size--) {
			*--d = *--s;
		}
	}
}
