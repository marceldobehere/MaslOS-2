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

    programWait(2000);

    globalPrintLn("A> Requesting Window...");
    Window* window = requestWindow();
    globalPrintLn("A> Requested Window!");

    if (window == NULL)
        return 0;

    globalPrint("A> Window ID: ");
    globalPrintLn(ConvertHexToString(window->ID));
    

    globalPrint("A> Window Title (1): \"");
    globalPrint(window->Title);
    globalPrintLn("\"");

    _Free(window->Title);
    window->Title = StrCopy("Hello World!");
    setWindow(window);
    
    globalPrint("A> Window Title (2): \"");
    globalPrint(window->Title);
    globalPrintLn("\"");


    programWait(2000);

    while (true)
    {
        _Free(window->Title);
        window->Title = StrCopy("AAA");
        setWindow(window);

        _Free(window->Title);
        window->Title = StrCopy("BBB");
        setWindow(window);

        programYield();

        // Check for mem leaks
        // serialPrint("A> Used Heap Count: ");
        // serialPrintLn(to_string(Heap::GlobalHeapManager->_usedHeapCount));
    }

    return 0;
}
