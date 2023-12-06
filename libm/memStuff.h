#pragma once
#include <stdint.h>
#include <stddef.h>

#ifndef _KERNEL_SRC


void _memcpy(void* src, void* dest, uint64_t size);
void _memset(void* dest, uint8_t value, uint64_t size);

#else

#include "../kernel/kernelStuff/memory/memory.h"

#endif

void _memmove(void* src, void* dest, uint64_t size);