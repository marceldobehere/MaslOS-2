#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>
#include <libm/keyboard.h>
#include <libm/stdio/stdio.h>

using namespace STDIO;

int main(int argc, const char** argv)
{
    initStdio(true);

    println("Enter your name:");
    const char* input = readLine();
    printlnf("\nHello, %s!", input);
    _Free(input);

    programWait(1500);

    return 0;
}