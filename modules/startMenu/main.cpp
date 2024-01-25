#include "main.h"
#include <libm/gui/guiStuff/components/text/textComponent.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/cstr.h>

using namespace GuiComponentStuff;

void StartMenuButtonClick(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo info);

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

    int sW = 200;
    int sH = 380;

    // Set the width and height to sWxsH
    window->Dimensions.width = sW;
    window->Dimensions.height = sH;
    window->Dimensions.x = 1;
    window->Dimensions.y = envData->globalFrameBuffer->Height - window->Dimensions.height - 1 - TASKBAR_HEIGHT;
    window->ShowTitleBar = false;
    window->ShowBorder = true;
    window->Resizeable = false;
    window->Moveable = false;
    window->SelectedBorderColor = Colors.orange;

    // Actually send the changes to the desktop
    setWindow(window);

    // Create the GUI Instance and initialize it
    GuiInstance* gui = new GuiInstance(window);
    gui->Init();
    gui->screen->backgroundColor = Colors.black;

    { 
        {
            GuiComponentStuff::TextComponent* txt = new GuiComponentStuff::TextComponent
            (
                gui->screen,
                Colors.tblack, Colors.white, 
                "Start Menu", 
                GuiComponentStuff::Position(0, 10)
            );
            txt->position.x = (sW - txt->GetActualComponentSize().FixedX) / 2;

            gui->screen->children->Add(txt);
        }
    

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Pong.maab", 
            Colors.cyan, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white, 
            GuiComponentStuff::ComponentSize(80, 20),
            GuiComponentStuff::Position(0, 40), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1001;
            
            gui->screen->children->Add(btn);
        }
        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Mandelbrot.maab", 
            Colors.cyan, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white, 
            GuiComponentStuff::ComponentSize(128, 20),
            GuiComponentStuff::Position(0, 60), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1002;
            
            gui->screen->children->Add(btn);
        }
        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Alert.maab", 
            Colors.cyan, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white, 
            GuiComponentStuff::ComponentSize(88, 20),
            GuiComponentStuff::Position(0, 80), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1003;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("New Terminal", 
            Colors.bgray, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(104, 20),
            GuiComponentStuff::Position(0, 120), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1004;
            
            gui->screen->children->Add(btn);
        }

        // {
        //     GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Task Manager", 
        //     Colors.bgray, Colors.yellow, Colors.black, 
        //     Colors.black, Colors.black, Colors.white,
        //     GuiComponentStuff::ComponentSize(104, 20),
        //     GuiComponentStuff::Position(0, 140), gui->screen
        //     );
        //     btn->mouseClickedCallBack = StartMenuButtonClick;
        //     btn->id = 1005;
            
        //     gui->screen->children->Add(btn);
        // }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Explorer", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(72, 20),
            GuiComponentStuff::Position(0, 180), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1006;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Notepad", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(64, 20),
            GuiComponentStuff::Position(0, 200), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1007;
            
            gui->screen->children->Add(btn);
        }


        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Image Viewer", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(104, 20),
            GuiComponentStuff::Position(0, 220), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1008;
            
            gui->screen->children->Add(btn);
        }

        // {
        //     GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Music Player", 
        //     Colors.bgreen, Colors.yellow, Colors.black, 
        //     Colors.black, Colors.black, Colors.white,
        //     GuiComponentStuff::ComponentSize(104, 20),
        //     GuiComponentStuff::Position(0, 240), gui->screen
        //     );
        //     btn->mouseClickedCallBack = StartMenuButtonClick;
        //     btn->id = 1009;
            
        //     gui->screen->children->Add(btn);
        // }

        // {
        //     GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Magnifier", 
        //     Colors.bgreen, Colors.yellow, Colors.black, 
        //     Colors.black, Colors.black, Colors.white,
        //     GuiComponentStuff::ComponentSize(80, 20),
        //     GuiComponentStuff::Position(0, 260), gui->screen
        //     );
        //     btn->mouseClickedCallBack = StartMenuButtonClick;
        //     btn->id = 1010;
            
        //     gui->screen->children->Add(btn);
        // }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Paint", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(48, 20),
            GuiComponentStuff::Position(0, 240), gui->screen//GuiComponentStuff::Position(0, 280), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1011;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Tic Tac Toe", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(96, 20),
            GuiComponentStuff::Position(0, 260), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1012;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Flappy Rocc", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(96, 20),
            GuiComponentStuff::Position(0, 160), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1013;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("DOOM", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(40, 20),
            GuiComponentStuff::Position(0, 280), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1014;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Minesweeper", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(96, 20),
            GuiComponentStuff::Position(0, 300), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1015;
            
            gui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("Connect Four", 
            Colors.bgreen, Colors.yellow, Colors.black, 
            Colors.black, Colors.black, Colors.white,
            GuiComponentStuff::ComponentSize(104, 20),
            GuiComponentStuff::Position(0, 320), gui->screen
            );
            btn->mouseClickedCallBack = StartMenuButtonClick;
            btn->id = 1016;
            
            gui->screen->children->Add(btn);
        }
    }


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


void StartMenuButtonClick(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo info)
{
    if (true)//(comp->id >= 1001 && comp->id <= 1007)
    {
        // PONG
        const char* BLEHUS_TITLE = "App Terminal Window";
        const char* BLEHUS_CMD   = "echo \"Unknown App lol\"";
        bool BLEHUS_CLOSE = true;
        bool BLEHUS_HIDE = false;

        if (comp->id == 1001)
        {
            const char* str = "bruh:programs/maab/assets/pong.maab";
            startProcess("bruh:programs/maab/maab.elf", 1, &str, "");
        }

        if (comp->id == 1002)
        {
            const char* str = "bruh:programs/maab/assets/maalbrot.maab";
            startProcess("bruh:programs/maab/maab.elf", 1, &str, "");
        }

        if (comp->id == 1003)
        {
            const char* str = "bruh:programs/maab/assets/alert.maab";
            startProcess("bruh:programs/maab/maab.elf", 1, &str, "");
        }

        if (comp->id == 1004)
        {
            startProcess("bruh:programs/shell/shell.elf", 0, NULL, "");
        }

        if (comp->id == 1005)
        {
            // BLEHUS_TITLE = "Task Manager Window";
            // BLEHUS_CMD   = "taskmgr";
        }

        if (comp->id == 1006)
        {
            startProcess("bruh:programs/explorer/explorer.elf", 0, NULL, "");
        }

        if (comp->id == 1007)
        {
            startProcess("bruh:programs/notepad/notepad.elf", 0, NULL, "");
        }

        if (comp->id == 1008)
        {
            startProcess("bruh:programs/imgView/imgView.elf", 0, NULL, "");
        }

        if (comp->id == 1009)
        {
            // BLEHUS_TITLE = "Music Player";
            // BLEHUS_CMD   = "music";
            // BLEHUS_HIDE = true;
        }

        if (comp->id == 1010)
        {
            // BLEHUS_TITLE = "Magnifier";
            // BLEHUS_CMD   = "mag";
            // BLEHUS_HIDE = true;
        }

        if (comp->id == 1011)
        {
            startProcess("bruh:programs/paint/paint.elf", 0, NULL, "");
        }

        if (comp->id == 1012)
        {
            startProcess("bruh:programs/ticTacToe/ticTacToe.elf", 0, NULL, "");
        }

        if (comp->id == 1013)
        {
            startProcess("bruh:programs/flappyRocc/flappyRocc.elf", 0, NULL, "");
        }

        if (comp->id == 1014)
        {
            startProcess("bruh:programs/doom/doom.elf", 0, NULL, "");
        }

        if (comp->id == 1015)
        {
            startProcess("bruh:programs/minesweeper/minesweeper.elf", 0, NULL, "");
        }

        if (comp->id == 1016)
        {
            startProcess("bruh:programs/connectFour/connectFour.elf", 0, NULL, "");
        }

        // //Window* oldActive = activeWindow;
        // Window* mainWindow = (Window*)_Malloc(sizeof(Window), "App Window");
        // TerminalInstance* terminal = new TerminalInstance(&guestUser);
        // *(mainWindow) = Window((DefaultInstance*)terminal, Size(500, 500), Position(50, 50), BLEHUS_TITLE, true, true, true);
        // mainWindow->hidden = BLEHUS_HIDE;
        // mainWindow->oldHidden = !BLEHUS_HIDE;
        
        // osData.windows.Add(mainWindow);
        // terminal->SetWindow(mainWindow);
        // terminal->closeWindowAfterTask = BLEHUS_CLOSE;
        // ((TerminalInstance*)mainWindow->instance)->Cls();
        // //KeyboardPrintStart(mainWindow);
        // //((TerminalInstance*)mainWindow->instance)->KeyboardPrintStart();
        // if (!BLEHUS_HIDE)
        //     osData.windowsToGetActive.Enqueue(mainWindow);

        // //((NewTerminalInstance*)terminal->newTermInstance)->Println(BLEHUS_CMD);
        // {
        //     int i = 0;
        //     for (; BLEHUS_CMD[i] != 0; i++)
        //         terminal->terminalInput[i] = BLEHUS_CMD[i];
        //     terminal->terminalInput[i] = 0;
        //     terminal->userlen = i;
        // }
        // terminal->tasks.Add(NewEnterTask(terminal));
    }
}