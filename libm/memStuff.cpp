#include "memStuff.h"


void _memcpy_128(void* src, void* dest, int64_t size)
{
	// __uint128_t is 16 bytes
	auto _src = (__uint128_t*)src;
	auto _dest = (__uint128_t*)dest;
	size >>= 4; // size /= 16
	while (size--)
		*(_dest++) = *(_src++);
}

void _memcpy(void* src, void* dest, uint64_t size)
{
	if (size & 0xFFE0)//(size >= 32)
	{
		uint64_t s2 = size & 0xFFFFFFF0;
		_memcpy_128(src, dest, s2);
		if (!s2)
			return;
		src = (void*)((uint64_t)src + s2);
		dest = (void*)((uint64_t)dest + s2);
		size = size & 0xF;
	}

    const char* _src  = (const char*)src;
    char* _dest = (char*)dest;
    while (size--)
        *(_dest++) = *(_src++);
}

void _memset_128(void* dest, uint8_t value, int64_t size)
{
	// __uint128_t is 16 bytes
	auto _dest = (__uint128_t*)dest;

	// Create a 128 bit value with the byte value in each byte
	__uint128_t val = value;
	val |= val << 8;
	val |= val << 16;
	val |= val << 32;
	val |= val << 64;

	size >>= 4; // size /= 16
	while (size--)
		*(_dest++) = val;
}

void _memset(void* dest, uint8_t value, uint64_t size)
{
	if (size & 0xFFE0)//(size >= 32)
	{
		uint64_t s2 = size & 0xFFFFFFF0;
		_memset_128(dest, value, s2);
		if (!s2)
			return;
		dest = (void*)((uint64_t)dest + s2);
		size = size & 0xF;
	}

    char* d = (char*)dest;
    for (uint64_t i = 0; i < size; i++)
        *(d++) = value;
}


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
