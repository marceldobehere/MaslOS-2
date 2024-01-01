#include "main.h"

using namespace GuiComponentStuff;

int main(int argc, char** argv)
{
    if (envData == NULL)
        return -1;

    if (envData->globalFrameBuffer == NULL)
        return -1;
        

    initWindowManagerStuff();

    // Request Window
    Window* window = requestWindow();
    if (window == NULL)
        return -1;
    
    // Set the title
    _Free(window->Title);
    window->Title = StrCopy("Start Menu");

    // Set the width and height to 400x160
    window->Dimensions.width = 160;
    window->Dimensions.height = 400;
    window->Dimensions.x = 1;
    window->Dimensions.y = envData->globalFrameBuffer->Height - window->Dimensions.height - 1 - TASKBAR_HEIGHT;
    window->ShowTitleBar = false;
    window->ShowBorder = true;
    window->Resizeable = false;
    window->Moveable = false;

    // Actually send the changes to the desktop
    setWindow(window);

    // Create the GUI Instance and initialize it
    GuiInstance* gui = new GuiInstance(window);
    gui->Init();


    // Our program should run as long as our main window is open
    while (!CheckForWindowClosed(window))
    {
        // Update and render the frame
        gui->Render(true);

        // Wait until we get a message
        programWaitMsg();
    }

    return 0;
}