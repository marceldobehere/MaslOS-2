#ifndef _KERNEL_SRC

#include <libm/syscallManager.h>
#include <libm/rnd/rnd.h>

extern "C" int main();

extern "C" void _start()
{
    uint64_t a = randomUint64();
    uint64_t b = randomUint64();
    RND::RandomInit(a, b);

    int res = main();
    proramExit(res);
}

#endif