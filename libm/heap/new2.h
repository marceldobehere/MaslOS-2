#ifndef _KERNEL_SRC

#pragma once
#include <stddef.h>

void* _Ymalloc(size_t size, const char* func, const char* file, int line);

#endif