#ifndef _KERNEL_SRC

#include "new2.h"
#include "heap.h"

void* _Ymalloc(size_t size, const char* func, const char* file, int line)
{
    return Heap::GlobalHeapManager->_Xmalloc(size, func, file, line);
}

#endif