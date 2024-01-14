#include <libm/syscallManager.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/gui/guiInstance.h>


int main(int argc, char** argv)
{
    initWindowManagerStuff();
    Window* window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Hello World!");
    setWindow(window);

    programWait(200);
    return 0;
}
