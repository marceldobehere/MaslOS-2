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

    serialPrintLn("> CLIENT STARTED");
    int cInt;
    while (true)
    {
        cInt = read();
        if (cInt != -1)
        {
            char c = (char)cInt;
            serialPrint("> Got char: ");
            char buf[2];
            buf[0] = c;
            buf[1] = '\0';
            serialPrintLn(buf);
        }
    }

    return 0;
}