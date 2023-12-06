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
    uint64_t pid = startProcess("bruh:programs/testoLogClient/testoLogClient.elf", 0, NULL, "");

    StdioInst child = initStdio(pid);

    serialPrintLn("> SERVER STARTED");
    print("Hello from parent\n\r", child);

    while (true)
        ;

    return 0;
}