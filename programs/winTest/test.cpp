#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/experimental/RelocatableAllocator.h>
#include <libm/experimental/AutoFree.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>

char buffer[512];

int main()
{
    int argc = getArgC();
    char **argv = getArgV();
    ENV_DATA *env = getEnvData();
    initWindowManagerStuff();
    
    programWait(1000);
    
    globalPrintLn("Hello from window Test");

    uint64_t pid = getPid();
    globalPrint("A> THIS PID: ");
    globalPrintLn(to_string(pid));

    globalPrint("A> DESKTOP PID: ");
    globalPrintLn(to_string(desktopPID));

    programWait(5000);

    globalPrint("A> Requesting Window...");

    Window* window = requestWindow();

    if (window != NULL)
    {
        globalPrint("A> Window ID: ");
        globalPrintLn(to_string(window->ID));
    }

    return 0;
}
