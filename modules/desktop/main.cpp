#include "main.h"
#include <libm/rendering/virtualRenderer.h>
#include <libm/cstrTools.h>
#include "wmStuff.h"
#include "mouseStuff.h"
#include <libm/msgPackets/mousePacket/mousePacket.h>
#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/windowBufferUpdatePacket/windowBufferUpdatePacket.h>
#include <libm/queue/queue_windowBufferUpdate.h>
#include <libm/msgPackets/windowObjPacket/windowObjPacket.h>

TempRenderer* actualScreenRenderer;
Framebuffer* actualScreenFramebuffer;
Framebuffer* mainBuffer;
PointerBuffer* pointerBuffer;

uint32_t defaultBackgroundColor = 0xFF202030;
bool drawBackground = false;
Framebuffer* backgroundImage = NULL;

Framebuffer* taskbar;

List<Window*>* windows;
Window* activeWindow;
Window* currentActionWindow;

MPoint MousePosition;
MPoint oldMousePos;

Queue<WindowBufferUpdatePacket*>* updateFramePackets;

void InitStuff()
{
    ENV_DATA* env = getEnvData();

    actualScreenFramebuffer = env->globalFrameBuffer;
    actualScreenRenderer = new TempRenderer(actualScreenFramebuffer, env->globalFont);
    VirtualRenderer::psf1_font = env->globalFont;

    {
        mainBuffer = new Framebuffer();
        mainBuffer->Width = actualScreenFramebuffer->Width;
        mainBuffer->Height = actualScreenFramebuffer->Height;
        mainBuffer->PixelsPerScanLine = mainBuffer->Width;
        mainBuffer->BufferSize = mainBuffer->Width * mainBuffer->Height * 4;
        mainBuffer->BaseAddress = _Malloc(mainBuffer->BufferSize, "Main Buffer");
        _memset(mainBuffer->BaseAddress, 0, mainBuffer->BufferSize);
    }

    {
        pointerBuffer = new PointerBuffer();
        pointerBuffer->Width = actualScreenFramebuffer->Width;
        pointerBuffer->Height = actualScreenFramebuffer->Height;
        pointerBuffer->BufferSize = pointerBuffer->Width * pointerBuffer->Height * sizeof(uint32_t*);
        pointerBuffer->BaseAddress = (uint32_t**)_Malloc(pointerBuffer->BufferSize, "Pointer Buffer");
        for (int y = 0; y < pointerBuffer->Height; y++)
            for (int x = 0; x < pointerBuffer->Width; x++)
                pointerBuffer->BaseAddress[x + y * pointerBuffer->Width] = &defaultBackgroundColor;
    }
    
    {
        taskbar = new Framebuffer();
        taskbar->Width = actualScreenFramebuffer->Width;
        taskbar->Height = 24;
        taskbar->PixelsPerScanLine = taskbar->Width;
        taskbar->BufferSize = taskbar->Width * taskbar->Height * 4;
        taskbar->BaseAddress = _Malloc(taskbar->BufferSize, "Taskbar Buffer");
        for (int y = 0; y < taskbar->Height; y++)
            for (int x = 0; x < taskbar->Width; x++)
                ((uint32_t*)taskbar->BaseAddress)[x + y * taskbar->PixelsPerScanLine] = Colors.dblue;
    }

    activeWindow = NULL;
    currentActionWindow = NULL;

    MousePosition.x = 0;
    MousePosition.y = 0;

    oldMousePos = MousePosition;
    

    windows = new List<Window*>(5);

    updateFramePackets = new Queue<WindowBufferUpdatePacket*>(5);
}

void PrintFPS(int fps, int aFps, int frameTime, int breakTime, int totalTime)
{
    actualScreenRenderer->CursorPosition.x = 0;
    actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 64;
    
    actualScreenRenderer->Clear(
        0, 
        actualScreenRenderer->CursorPosition.y, 

        300, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );

    actualScreenRenderer->Print("FPS: {}", to_string(aFps), Colors.yellow);
    actualScreenRenderer->Print(" ({})", to_string(fps), Colors.yellow);
    actualScreenRenderer->Print(" ({} /", to_string(frameTime), Colors.yellow);
    actualScreenRenderer->Print(" {} /", to_string(breakTime), Colors.yellow);
    actualScreenRenderer->Print(" {})", to_string((totalTime)), Colors.yellow);

    actualScreenRenderer->Clear(
        300, 
        actualScreenRenderer->CursorPosition.y, 

        500, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );

    actualScreenRenderer->CursorPosition.x = 300;

    actualScreenRenderer->Print("MALLOC: {}", to_string(Heap::GlobalHeapManager->_usedHeapCount), Colors.yellow);  
}



int main()
{
    serialPrintLn("Starting Desktop");

    programSetPriority(2);

    programWait(100);

    InitStuff();

    Window* window = new Window(50, 30, 200, 200, "Test Window 1", RND::RandomInt(), getPid());
    windows->Add(window);
    _memset(window->Buffer->BaseAddress, 0x80, window->Buffer->BufferSize);

    {
        WindowBufferUpdatePacket* packet = new WindowBufferUpdatePacket(
            0, 0, window->Buffer->Width, window->Buffer->Height, 
            window->ID,
            (uint32_t*)window->Buffer->BaseAddress
        );

        GenericMessagePacket* msg = packet->ToGenericMessagePacket();

        msgSendMessage(msg, getPid());

        msg->Free();
        _Free(msg);

        packet->Free();
        _Free(packet);
    }

    activeWindow = window;

    actualScreenRenderer->Clear(Colors.black);
    actualScreenRenderer->Println("WINDOW 0x{}", ConvertHexToString((uint64_t)window->Buffer), Colors.yellow);
    


    Clear(true);
    RenderWindows();
    ActuallyRenderWindow(window);

    DrawFrame();



    serialPrintLn("Starting Main Loop");
    
    const int frameCount = 60;
    while (true)
    {
        uint64_t frameTime = 0;
        uint64_t breakTime = 0;
        
        uint64_t _startTime = envGetTimeMs();
        
        for (int i = 0; i < frameCount; i++)
        {
            uint64_t startTime = envGetTimeMs();
            DrawFrame();
            uint64_t endTime = envGetTimeMs();

            frameTime += endTime - startTime;

            //programWait(10);
            programYield();

            uint64_t endTime2 = envGetTimeMs();

            breakTime += endTime2 - endTime;

            //programYield();
        }

        uint64_t _endTime = envGetTimeMs();

        uint64_t totalTime = _endTime - _startTime;

        if (frameTime == 0)
            frameTime = 1;
        if (totalTime == 0)
            totalTime = 1;
        int fps = (int)((frameCount * 1000) / frameTime);
        int aFps = (int)((frameCount * 1000) / totalTime);

        frameTime = (frameTime * 1000) / frameCount;
        breakTime = (breakTime * 1000) / frameCount;
        totalTime = (totalTime * 1000) / frameCount;

        PrintFPS(fps, aFps, frameTime, breakTime, totalTime);

        // Check for mem leaks
        // serialPrint("B> Used Heap Count: ");
        // serialPrintLn(to_string(Heap::GlobalHeapManager->_usedHeapCount));
    }

    return 0;
}



void DrawFrame()
{
    uint32_t rndCol = (uint32_t)RND::RandomInt();

    updateFramePackets->Clear();

    bool doUpdate = false;
    int msgCount = min(msgGetCount(), 10);
    for (int i = 0; i < msgCount; i++)
    {
        GenericMessagePacket* msg = msgGetMessage();
        if (msg == NULL)
            break;
        if (msg->Type == MessagePacketType::MOUSE_EVENT)
        {
            MouseMessagePacket* mouseMsg = (MouseMessagePacket*)msg->Data;
            if (mouseMsg->Type == MouseMessagePacketType::MOUSE_MOVE)
            {
                MousePosition.x = mouseMsg->MouseX;
                MousePosition.y = mouseMsg->MouseY;
            }
        }
        else if (msg->Type == MessagePacketType::KEY_EVENT)
        {
            KeyMessagePacket* keyMsg = (KeyMessagePacket*)msg->Data;
            if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED)
            {
                actualScreenRenderer->CursorPosition.x = 0;
                actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 128;

                actualScreenRenderer->Clear(
                    0, actualScreenRenderer->CursorPosition.y, 
                    160, actualScreenRenderer->CursorPosition.y + 16, 
                    Colors.black
                );

                actualScreenRenderer->Println("> KEY {} HELD", to_string((int)keyMsg->Scancode), Colors.white);
            }
            else if (keyMsg->Type == KeyMessagePacketType::KEY_RELEASE)
            {
                actualScreenRenderer->CursorPosition.x = 0;
                actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 128;

                actualScreenRenderer->Clear(
                    0, actualScreenRenderer->CursorPosition.y, 
                    160, actualScreenRenderer->CursorPosition.y + 16, 
                    Colors.black
                );

                actualScreenRenderer->Println("> KEY {} RELEASED", to_string((int)keyMsg->Scancode), Colors.white);
            }
        }
        else if (msg->Type == MessagePacketType::WINDOW_BUFFER_EVENT)
        {
            WindowBufferUpdatePacket* windowBufferUpdatePacket = new WindowBufferUpdatePacket(msg);

            updateFramePackets->Enqueue(windowBufferUpdatePacket);
        }
        else if (msg->Type == MessagePacketType::WINDOW_CREATE_EVENT && msg->Size == 0)
        {
            uint64_t pidFrom = msg->FromPID;
            uint64_t newWindowId = RND::RandomInt();

            Window* window = new Window(350, 30, 400, 200, "Created Test Window", newWindowId, pidFrom);
            windows->Add(window);
            _memset(window->Buffer->BaseAddress, 0x20, window->Buffer->BufferSize);

            //Clear(true);
            //RenderWindows();
            ActuallyRenderWindow(window);

            if (pidFrom != getPid())
            {
                serialPrintLn("Sending Window ID");
                serialPrint("Win ID: ");
                serialPrintLn(ConvertHexToString(newWindowId));
                serialPrint("From PID: ");
                serialPrintLn(to_string(getPid()));
                serialPrint("To PID: ");
                serialPrintLn(to_string(pidFrom));
                GenericMessagePacket* response = new GenericMessagePacket(MessagePacketType::WINDOW_CREATE_EVENT, (uint8_t*)&newWindowId, 8);
                msgSendMessage(response, pidFrom);
                response->Free();
                _Free(response);
            }
        }
        else if (msg->Type == MessagePacketType::WINDOW_GET_EVENT)
        {
            WindowObjectPacket* winObjPacketFrom = new WindowObjectPacket(msg);
            Window* fromWind = winObjPacketFrom->PartialWindow;
            uint64_t winId = 0;
            if (fromWind != NULL)
            {
                winId = fromWind->ID;   
            }

            // serialPrint("> WIN ID: ");
            // serialPrintLn(ConvertHexToString(winId));

            if (!winObjPacketFrom->Set)
            {
                Window* win = NULL;
                for (int i = 0; i < windows->GetCount(); i++)
                {
                    Window* tWin = windows->ElementAt(i);
                    if (tWin->ID == winId)
                    {
                        win = tWin;
                        break;
                    }
                }

                if (win != NULL)
                {
                    // serialPrintLn("> WIN GET EVENT");
                    WindowObjectPacket* winObjPacketTo = new WindowObjectPacket(win, false);
                    GenericMessagePacket* sendMsg = winObjPacketTo->ToGenericMessagePacket();

                    // serialPrintLn("> Sending Window");
                    msgSendMessage(sendMsg, msg->FromPID);
                    
                    sendMsg->Free();
                    _Free(sendMsg);

                    winObjPacketTo->Free();
                    _Free(winObjPacketTo);
                }
                else
                {
                    // serialPrintLn("> Window not found");
                }
            }

            if (fromWind != NULL)
            {
                fromWind->Free();
                _Free(fromWind);
            }

            winObjPacketFrom->Free();
            _Free(winObjPacketFrom);
        }
        else if (msg->Type == MessagePacketType::WINDOW_SET_EVENT)
        {
            WindowObjectPacket* winObjPacketFrom = new WindowObjectPacket(msg);
            Window* fromWind = winObjPacketFrom->PartialWindow;
            uint64_t winId = 0;
            if (fromWind != NULL)
            {
                winId = fromWind->ID;   
            }

            // serialPrint("> WIN ID: ");
            // serialPrintLn(ConvertHexToString(winId));

            if (winObjPacketFrom->Set)
            {
                Window* win = NULL;
                for (int i = 0; i < windows->GetCount(); i++)
                {
                    Window* tWin = windows->ElementAt(i);
                    if (tWin->ID == winId)
                    {
                        win = tWin;
                        break;
                    }
                }

                if (win != NULL)
                {
                    // serialPrintLn("> WIN SET EVENT");
                    win->UpdateUsingPartialWindow(fromWind, false);
                    win->UpdateCheck();
                }
                else
                {
                    // serialPrintLn("> Window not found");
                }
            }

            if (fromWind != NULL)
            {
                fromWind->Free();
                _Free(fromWind);
            }

            winObjPacketFrom->Free();
            _Free(winObjPacketFrom);
        }

        msg->Free();
        _Free(msg);
    }

    doUpdate |= updateFramePackets->GetCount() > 0;

    // check for window updates
    for (int i = 0; i < windows->GetCount(); i++)
    {
        Window* window = windows->ElementAt(i);
        window->UpdateCheck();

        if (window->Updates->GetCount() > 0)
        {
            doUpdate = true;

            for (int j = 0; j < window->Updates->GetCount(); j++)
            {
                WindowUpdate update = window->Updates->ElementAt(j);

                UpdatePointerRect(
                    window->Dimensions.x + update.x1, 
                    window->Dimensions.y + update.y1, 
                    
                    window->Dimensions.x + update.x2, 
                    window->Dimensions.y + update.y2
                );

                RenderActualSquare(
                    window->Dimensions.x + update.x1, 
                    window->Dimensions.y + update.y1, 
                    
                    window->Dimensions.x + update.x2, 
                    window->Dimensions.y + update.y2
                );
            }

            window->Updates->Clear();
        }
    }

    //Taskbar::RenderTaskbar();



    // Handle mouse
    //ProcessMousePackets();
    //MousePosition.x = (MousePosition.x + 5) % 600;
    //MousePosition.y = (MousePosition.y + 7) % 400;

    //doUpdate = true;

    doUpdate |= MousePosition != oldMousePos;

    if (!doUpdate)
        return;

    RenderActualSquare(
        oldMousePos.x, 
        oldMousePos.y, 
        
        oldMousePos.x + 16, 
        oldMousePos.y + 16
    );

    // Draw Mouse
    MPoint tempMousePos = MousePosition;
    DrawMousePointer(tempMousePos, pointerBuffer);
    
    //Render();

    while (updateFramePackets->GetCount() > 0)
    {
        WindowBufferUpdatePacket* packet = updateFramePackets->Dequeue();

        // get actual window
        Window* win = NULL;
        for (int i = 0; i < windows->GetCount(); i++)
            if (windows->ElementAt(i)->ID == packet->WindowId)
            {
                win = windows->ElementAt(i);
                break;
            }
        

        if (win != NULL)
        {
            uint32_t* winBuf = (uint32_t*)win->Buffer->BaseAddress;
            // update actual window buffer
            for (int y = 0; y < packet->Height; y++)
                for (int x = 0; x < packet->Width; x++)
                {
                    int aX = packet->X + x;
                    int aY = packet->Y + y;

                    if (x < 0 || y < 0 || x >= win->Buffer->Width || 
                        y >= win->Buffer->Height)
                        continue;
                    if (aX < 0 || aY < 0 || aX >= win->Buffer->Width || 
                        aY >= win->Buffer->Height)
                        continue;

                    winBuf[aX + aY * win->Buffer->Width] = packet->Buffer[x + y * packet->Width];
                }

            // render changes to screen
            RenderActualSquare(
                win->Dimensions.x + packet->X, 
                win->Dimensions.y + packet->Y, 
                
                win->Dimensions.x + packet->X + packet->Width - 1, 
                win->Dimensions.y + packet->Y + packet->Height - 1
            );
        }

        
        packet->Free();
        _Free(packet);
    }

    RenderActualSquare(
        tempMousePos.x, 
        tempMousePos.y, 
        
        tempMousePos.x + 16, 
        tempMousePos.y + 16
    );
    oldMousePos = tempMousePos;

    // Remove Mouse
    UpdatePointerRect(tempMousePos.x - 32, tempMousePos.y - 32, tempMousePos.x + 64, tempMousePos.y + 64);



    //actualScreenRenderer->Clear(300, 300, 320, 320, rndCol);
}


/*
HandleKeyboardList(20);



if (bgm != osData.drawBackground)
{
    osData.windowPointerThing->RenderWindows();
    bgm = osData.drawBackground;
}

AddToStack();
if (osData.windowsToGetActive.GetCount() > 0)
{
    Window* w = osData.windowsToGetActive.Dequeue();

    Window* oldActive = activeWindow;
    activeWindow = w;          
    if (w != NULL)
    {
        w->moveToFront = true;
        w->hidden = false;
        if (w == osData.debugTerminalWindow)
            osData.showDebugterminal = true;
    }

    if (oldActive != NULL)
        osData.windowPointerThing->UpdateWindowBorder(oldActive);
}

RemoveFromStack();

AddToStack();
{
    uint64_t tS = PIT::TimeSinceBootMicroS();

    if (activeWindow != NULL)
    {
        updateBorder = true;
        if (activeWindow->moveToFront)
        {
            activeWindow->moveToFront = false;
            int index = osData.windows.GetIndexOf(activeWindow);
            if (index == osData.windows.GetCount() - 1)
            {
                osData.windowPointerThing->UpdateWindowBorder(activeWindow);
                osData.windowPointerThing->RenderWindow(activeWindow);
            }
            else if (index != -1)
            {
                Window* oldActive = osData.windows[osData.windows.GetCount() - 1];
                osData.windows.RemoveAt(index);
                osData.windows.Add(activeWindow);
                
                osData.windowPointerThing->UpdateWindowBorder(oldActive);

                osData.windowPointerThing->RenderWindow(activeWindow);
                osData.windowPointerThing->UpdateWindowBorder(activeWindow);
            }
        }
    }
    else
    {
        if (updateBorder)
        {
            updateBorder = false;
            {
                Window* oldActive = osData.windows[osData.windows.GetCount() - 1];
                
                osData.windowPointerThing->UpdateWindowBorder(oldActive);
            }
        }
    }

    for (int i = 0; i < osData.windows.GetCount(); i++)
    {            
        Window* window = osData.windows[i];

        if (window == osData.debugTerminalWindow && !osData.showDebugterminal)
            continue;
        
        if (window->hidden != window->oldHidden)
        {
            window->oldHidden = window->hidden;
            osData.windowPointerThing->UpdatePointerRect(
                window->position.x - 1, 
                window->position.y - 23, 
                window->position.x + window->size.width, 
                window->position.y + window->size.height
                );
        }

        {
            int x1 = window->position.x - 1;
            int y1 = window->position.y - 23;
            int sx1 = window->size.width + 3;
            int sy1 = window->size.height + 25;

            bool update = false;

            int x2 = x1;
            int y2 = y1;
            int sx2 = sx1;
            int sy2 = sy2;

            if (window->maximize && window->oldMaximize)
            {
                if (window->position.x != 0 || window->position.y != 23 || 
                window->size.width != osData.windowPointerThing->actualScreenBuffer->Width ||
                window->size.height != osData.windowPointerThing->actualScreenBuffer->Height - 23)
                {
                    window->maximize = false;
                    
                    MPoint tMouse = MousePosition;
                    tMouse.x -= window->position.x;
                    tMouse.y -= window->position.y;

                    tMouse.x = (tMouse.x * window->oldPreMaxSize.width) / window->size.width;
                    tMouse.y = (tMouse.y * window->oldPreMaxSize.height) / window->size.height;

                    window->newPosition.x = MousePosition.x - tMouse.x;
                    window->newPosition.y = MousePosition.y - tMouse.y;

                    //window->newPosition = window->oldPreMaxPosition;
                    window->newSize = window->oldPreMaxSize;

                    window->showBorder = window->oldPreMaxBorder;
                    window->showTitleBar = window->oldPreMaxTitle;

                    window->oldMaximize = false;
                }
            }

            if (window->maximize != window->oldMaximize)
            {
                if (window->maximize)
                {
                    window->oldPreMaxPosition = window->position;
                    window->oldPreMaxSize = window->size;

                    window->oldPreMaxBorder = window->showBorder;
                    window->oldPreMaxTitle = window->showTitleBar;

                    window->newPosition = Position(0, 23);
                    window->newSize = Size(osData.windowPointerThing->actualScreenBuffer->Width, osData.windowPointerThing->actualScreenBuffer->Height - 23);

                    window->showBorder = false;
                    //window->showTitleBar = false;

                    window->oldMaximize = true;
                }
                else
                {
                    window->newPosition = window->oldPreMaxPosition;
                    window->newSize = window->oldPreMaxSize;

                    window->showBorder = window->oldPreMaxBorder;
                    window->showTitleBar = window->oldPreMaxTitle;

                    window->oldMaximize = false;
                }
            }


            Size nSize = window->newSize;
            Position nPos = window->newPosition;


            if (window->size != nSize)
            {
                window->Resize(nSize);
                {
                    x2 = window->position.x - 1;
                    y2 = window->position.y - 23;
                    sx2 = window->size.width + 3;
                    sy2 = window->size.height + 25;

                    update = true;
                }
            }

            if (window->position != nPos)
            {
                window->position = nPos;

                x2 = window->position.x - 1;
                y2 = window->position.y - 23;
                sx2 = window->size.width + 3;
                sy2 = window->size.height + 25;

                update = true;
            }

            if (update)
            {
                int rx1 = min(x1, x2);
                int ry1 = min(y1, y2);
                int rx2 = max(x1 + sx1, x2 + sx2);
                int ry2 = max(y1 + sy1, y2 + sy2);

                int AR = (rx2 - rx1) * (ry2 - ry1);
                int A1 = sx1 * sy1;
                int A2 = sx2 * sy2;

                if (AR <= A1+A2)
                {
                    osData.windowPointerThing->UpdatePointerRect(rx1, ry1, rx2, ry2);
                }
                else
                {
                    osData.windowPointerThing->UpdatePointerRect(x1, y1, x1 + sx1, y1 + sy1);
                    osData.windowPointerThing->UpdatePointerRect(x2, y2, x2 + sx2, y2 + sy2);
                }
            }
        }
        if (window->instance != NULL && !window->hidden && (activeWindow == window || frame % 5 == (i%3)))
        {
            if (window->instance->instanceType == InstanceType::Terminal)
            {
                TerminalInstance* termInst1 = (TerminalInstance*)window->instance;
                if (termInst1->newTermInstance != NULL)
                {
                    NewTerminalInstance* termInst2 = (NewTerminalInstance*)termInst1->newTermInstance;
                    termInst2->DoRender();
                }
            }   
            else if (window->instance->instanceType == InstanceType::GUI)
            {
                GuiInstance* guiInst = (GuiInstance*)window->instance;
                guiInst->Render();
            }   
        }
        
    }

    osStats.totalWindowUpdateTime = PIT::TimeSinceBootMicroS() - tS;
}
RemoveFromStack();

AddToStack();
{
    for (int i = 0; i < osData.windows.GetCount(); i++)
    {     
        Window* window = osData.windows[i];
        window->RenderStuff();
    }
}
RemoveFromStack();


{
    uint64_t tS = PIT::TimeSinceBootMicroS();
    AddToStack();
    Taskbar::RenderTaskbar();

    // Handle mouse
    AddToStack();
    ProcessMousePackets();
    RemoveFromStack();

    // Draw Mouse
    AddToStack();
    MPoint mPosOld = MousePosition;
    DrawMousePointer2(osData.windowPointerThing->virtualScreenBuffer, mPosOld);
    RemoveFromStack();

    // Render Screen
    AddToStack();
    osData.windowPointerThing->fps = fps;
    osData.windowPointerThing->Render();
    RemoveFromStack();

    // Remove Mouse
    AddToStack();
    osData.windowPointerThing->UpdatePointerRect(mPosOld.x - 32, mPosOld.y - 32, mPosOld.x + 64, mPosOld.y + 64);
    RemoveFromStack();

    osStats.totalRenderTime = PIT::TimeSinceBootMicroS() - tS;
    RemoveFromStack();
}
*/



