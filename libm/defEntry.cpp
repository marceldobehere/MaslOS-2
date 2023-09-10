#ifndef _KERNEL_SRC

#include <libm/syscallManager.h>
#include <libm/rnd/rnd.h>
#include <libm/heap/heap.h>

extern "C" int main();

extern "C" void _start()
{
    uint64_t a = randomUint64();
    uint64_t b = randomUint64();
    RND::RandomInit(a, b);
    
    void* tempMemStart = requestNextPage();
    *((Heap::HeapManager**)tempMemStart) = &Heap::GlobalHeapManager;

    Heap::GlobalHeapManager.InitializeHeap(4);

    int res = main();
    while (true)
        programExit(res);
}

#endif