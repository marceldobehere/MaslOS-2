#ifndef _KERNEL_SRC

#include <libm/syscallManager.h>

extern "C" int main();

extern "C" void _start()
{
    int res = main();
    exitProgram(res);
}

#endif