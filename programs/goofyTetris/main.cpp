#include "main.h"

GuiInstance* guiInstance;
Window* window;
CanvasComponent* canvas;

int main(int argc, const char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    // Update Window
    _Free(window->Title);
    window->Title = StrCopy("Goofy Tetris");
    window->Dimensions.width = 500;
    window->Dimensions.height = 500;
    window->Resizeable = false;
    setWindow(window);

    // Init Gui Instance
    guiInstance = new GuiInstance(window);
    guiInstance->Init();
    guiInstance->screen->backgroundColor = Colors.white;

    // Create Canvas
    guiInstance->CreateComponentWithId(1400, ComponentType::CANVAS);
    canvas = (CanvasComponent*)guiInstance->GetComponentFromId(1400);
    canvas->position.x = 0;
    canvas->position.y = 0;
    canvas->size.IsXFixed = false;
    canvas->size.ScaledX = 1;
    canvas->size.IsYFixed = false;
    canvas->size.ScaledY = 1;
    canvas->bgColor = Colors.white;

    // Init Canvas
    canvas->Clear();
    canvas->CheckUpdates();

    MainLoop();

    return 0;
}


void MainLoop()
{
    while (!CheckForWindowClosed(window))
    {
        guiInstance->Update();
        HandleFrame();
        guiInstance->Render(false);

        // Wait 20ms per frame
        programWait(20);
    }
}

void HandleFrame()
{
    uint64_t rndX = RND::RandomInt() % 300;
    uint64_t rndY = RND::RandomInt() % 300;
    uint32_t rndCol = (uint32_t)RND::RandomInt();
    canvas->DrawRect(rndX, rndY, 100, 50, rndCol, true);

    // All Scancodes are here https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1
    if (envGetKeyState(Key_Enter))
    {
        serialPrintLn("> ENTER WAS PRESSED!");
    }
}