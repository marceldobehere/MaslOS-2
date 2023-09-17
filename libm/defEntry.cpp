#ifndef _KERNEL_SRC

#include <libm/syscallManager.h>
#include <libm/rnd/rnd.h>
#include <libm/heap/heap.h>
#include <libm/memStuff.h>

extern "C" int main();


extern "C" void _start()
{
    uint64_t a = randomUint64();
    uint64_t b = randomUint64();
    RND::RandomInit(a, b);
    
    void* tempMemStart = requestNextPage();
    _memset(tempMemStart, 0, 0x1000);
    Heap::GlobalHeapManager = (Heap::HeapManager*)tempMemStart;

    Heap::GlobalHeapManager->InitializeHeap(4);

    int argc = getArgC();
    char **argv = getArgV();

    // globalPrintLn("GETTING ARGVS:");
    // for (int i = 0; i < argc; i++)
    // {
    //     globalPrint(" - ");
    //     globalPrintLn(argv[i]);
    //     _Free(argv[i]);
    // }
    // globalPrintLn("DONE GETTING ARGVS");
    // if (argv != NULL)
    //     _Free(argv);

    int res = main();
    while (true)
        programExit(res);
}

#endif