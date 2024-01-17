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
#include <libm/images/bitmapImage.h>
#include "taskbarConst.h"
#include <libm/fsStuff/extra/fsExtra.h>

TempRenderer* actualScreenRenderer;
Framebuffer* actualScreenFramebuffer;
Framebuffer* mainBuffer;
PointerBuffer* pointerBuffer;

uint32_t defaultBackgroundColor = Colors.black;
bool drawBackground = false;
Framebuffer* backgroundImage = NULL;

Framebuffer* taskbar;

List<void*>* windowIconEntries;
List<Window*>* windows;
List<Window*>* windowsToDelete;
List<Window*>* windowsUpdated;
Window* activeWindow;
Window* currentActionWindow;
Window* startMenuWindow;
uint64_t startMenuPid = 0;

ImageStuff::BitmapImage* windowButtonIcons[countOfButtonIcons];

ImageStuff::BitmapImage* internalWindowIcons[countOfWindowIcons];

MPoint MousePosition;
MPoint oldMousePos;

Queue<WindowBufferUpdatePacket*>* updateFramePackets;

Queue<WindowUpdate>* ScreenUpdates;
List<GenericMessagePacket*>* tempPackets;

uint64_t lastFrameTime = 0;

#include <libm/zips/basicZip.h>

#include "taskbar.h"

void InitStuff()
{
    ENV_DATA* env = getEnvData();

    windowIconEntries = new List<void*>();

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
        taskbar->Height = TASKBAR_HEIGHT;
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
    oldMousePos.x += 10;
    

    windows = new List<Window*>(5);
    windowsToDelete = new List<Window*>(3);
    windowsUpdated = new List<Window*>(5);
    tempPackets = new List<GenericMessagePacket*>(5);

    ScreenUpdates = new Queue<WindowUpdate>(20);

    updateFramePackets = new Queue<WindowBufferUpdatePacket*>(5);

    startMenuWindow = NULL;
    startMenuPid = 0;

    lastFrameTime = envGetTimeMs();



    const char* bgPath = "bruh:modules/desktop/assets/background.mbif";

    {
        char* buf;
        uint64_t size = 0;
        if (fsReadFile(bgPath, (void**)&buf, &size))
        {
            ImageStuff::BitmapImage* img = ImageStuff::ConvertBufferToBitmapImage(buf, size);
            if (img != NULL)
            {
                backgroundImage = new Framebuffer();
                backgroundImage->Width = img->width;
                backgroundImage->Height = img->height;
                backgroundImage->PixelsPerScanLine = backgroundImage->Width;
                backgroundImage->BufferSize = backgroundImage->Width * backgroundImage->Height * 4;
                backgroundImage->BaseAddress = _Malloc(backgroundImage->BufferSize, "Background Image Buffer");
                _memcpy(img->imageBuffer, backgroundImage->BaseAddress, backgroundImage->BufferSize);
                _Free(img->imageBuffer);
                _Free(img);
                drawBackground = true;
            }
            _Free(buf);
        }
    }

    const char* windowButtonPath = "bruh:modules/desktop/assets/windowButtons.mbzf";
    {
        char* buf;
        uint64_t size = 0;
        if (fsReadFile(windowButtonPath, (void**)&buf, &size))
        {
            ZipStuff::ZIPFile* zip = ZipStuff::ZIP::GetZIPFromBuffer(buf, size);

            for (int i = 0; i < countOfButtonIcons; i++)
                windowButtonIcons[i] = ImageStuff::ConvertFileToBitmapImage(ZipStuff::ZIP::GetFileFromFileName(zip, windowButtonIconNames[i]));
        }
        else
        {
            for (int i = 0; i < countOfButtonIcons; i++)    
                windowButtonIcons[i] = NULL;
        }
    }

    const char* windowIconsPath = "bruh:modules/desktop/assets/windowIcons.mbzf";
    {
        char* buf;
        uint64_t size = 0;
        if (fsReadFile(windowIconsPath, (void**)&buf, &size))
        {
            ZipStuff::ZIPFile* zip = ZipStuff::ZIP::GetZIPFromBuffer(buf, size);

            for (int i = 0; i < countOfWindowIcons; i++)
                internalWindowIcons[i] = ImageStuff::ConvertFileToBitmapImage(ZipStuff::ZIP::GetFileFromFileName(zip, windowIconNames[i]));
        }
        else
        {
            for (int i = 0; i < countOfWindowIcons; i++)    
                internalWindowIcons[i] = NULL;
        }
    }

    mouseZIP = NULL;
    {
        char* buf;
        uint64_t size = 0;
        if (fsReadFile("bruh:modules/desktop/assets/mouse.mbzf", (void**)&buf, &size))
        {
            mouseZIP = ZipStuff::ZIP::GetZIPFromBuffer(buf, size);
            //_Free(buf);
        }
    }

    ImageStuff::BitmapImage* mButton = NULL;
    const char* mButtonPath = "bruh:modules/desktop/assets/MButton.mbif";
    {
        char* buf;
        uint64_t size = 0;
        if (fsReadFile(mButtonPath, (void**)&buf, &size))
        {
            mButton = ImageStuff::ConvertBufferToBitmapImage(buf, size);
        }
    }

    ImageStuff::BitmapImage* mButtonS = NULL;
    const char* mButtonSPath = "bruh:modules/desktop/assets/MButtonS.mbif";
    {
        char* buf;
        uint64_t size = 0;
        if (fsReadFile(mButtonSPath, (void**)&buf, &size))
        {
            mButtonS = ImageStuff::ConvertBufferToBitmapImage(buf, size);
        }
    }

    Taskbar::InitTaskbar(mButton, mButtonS);
}

void PrintFPS(int fps, int aFps, int frameTime, int breakTime, int totalTime, uint64_t totalPixelCount, int frameCount)
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

    int tTime = totalTime;

    frameTime = (frameTime * 1000) / frameCount;
    breakTime = (breakTime * 1000) / frameCount;
    totalTime = (totalTime * 1000) / frameCount;
        
    actualScreenRenderer->Print("FPS: {}", to_string(aFps), Colors.yellow);
    actualScreenRenderer->Print(" ({})", to_string(fps), Colors.yellow);
    actualScreenRenderer->Print(" ({} /", to_string(frameTime), Colors.yellow);
    actualScreenRenderer->Print(" {} /", to_string(breakTime), Colors.yellow);
    actualScreenRenderer->Print(" {})", to_string((totalTime)), Colors.yellow);


    actualScreenRenderer->CursorPosition.x = 300;
    actualScreenRenderer->Clear(
        300, 
        actualScreenRenderer->CursorPosition.y, 

        500, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );
    actualScreenRenderer->Print("MALLOC: {}", to_string(Heap::GlobalHeapManager->_usedHeapCount), Colors.yellow);  


    actualScreenRenderer->CursorPosition.x = 500;
    actualScreenRenderer->Clear(
        500, 
        actualScreenRenderer->CursorPosition.y, 

        880, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );
    actualScreenRenderer->Print("PIXELS PER FRAME: {}", to_string(totalPixelCount / frameCount), Colors.yellow);

    totalTime = tTime;

    actualScreenRenderer->Print(" ({} PPS)", to_string((totalPixelCount * 1000) / totalTime), Colors.yellow);
}




int main(int argc, char** argv)
{
    serialPrintLn("Starting Desktop");
    programSetPriority(1);
    InitStuff();

    activeWindow = NULL;

    // Clear and Redraw
    //Clear(true);
    UpdatePointerRect(0, 0, actualScreenFramebuffer->Width - 1, actualScreenFramebuffer->Height - 1);
    RenderWindows();
    RenderActualSquare(0, 0, actualScreenFramebuffer->Width - 1, actualScreenFramebuffer->Height - 1);


    DrawFrame();

    serialPrintLn("Starting Main Loop");
    
    const int frameCount = 60;
    while (true)
    {
        uint64_t frameTime = 0;
        uint64_t breakTime = 0;
        uint64_t totalPixelCount = 0;
        
        uint64_t _startTime = envGetTimeMs();
        
        for (int i = 0; i < frameCount; i++)
        {
            uint64_t startTime = envGetTimeMs();
            totalPixelCount += DrawFrame();

            if ((i & 7) == 0)
                CheckForDeadWindows();

            uint64_t endTime = envGetTimeMs();

            frameTime += endTime - startTime;

            //programWait(10);
            //programYield();
            programWaitMsg();

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


        PrintFPS(fps, aFps, frameTime, breakTime, totalTime, totalPixelCount, frameCount);

        // Check for mem leaks
        // serialPrint("B> Used Heap Count: ");
        // serialPrintLn(to_string(Heap::GlobalHeapManager->_usedHeapCount));
    }

    return 0;
}

void CheckForDeadWindows()
{
    for (int i = 0; i < windows->GetCount(); i++)
    {
        Window* window = windows->ElementAt(i);
        if (!pidExists(window->PID))
        {
            AddWindowToBeRemoved(window);

            i--;
        }
    }   
}

void AddWindowToBeRemoved(Window* window)
{
    windowsToDelete->Add(window);

    int idx = windows->GetIndexOf(window);
    if (idx != -1)
        windows->RemoveAt(idx);

    for (int i = 0; i < windowIconEntries->GetCount(); i++)
    {
        WindowIconEntry* entry = (WindowIconEntry*)windowIconEntries->ElementAt(i);
        if (entry->window == window)
        {
            windowIconEntries->RemoveAt(i);
            entry->Free();
            _Free(entry);
            i--;
        }
    }

    if (activeWindow == window)
        activeWindow = NULL;
    if (startMenuWindow == window)
    {
        startMenuWindow = NULL;
        startMenuPid = 0;
    }
    if (Taskbar::activeTabWindow == window)
        Taskbar::activeTabWindow = NULL;
    if (currentActionWindow == window)
        currentActionWindow = NULL;
}

uint64_t DrawFrame()
{
    uint64_t totalPixelCount = 0;
    uint32_t rndCol = (uint32_t)RND::RandomInt();

    updateFramePackets->Clear();
    windowsUpdated->Clear();

    if (startMenuPid == 0 || (startMenuWindow == NULL && !pidExists(startMenuPid)))
        startMenuPid = envGetStartMenuPid();
    if (startMenuPid != 0 && startMenuWindow == NULL)
    {
        for (int i = 0; i < windows->GetCount(); i++)
        {
            Window* window = windows->ElementAt(i);
            if (window->PID == startMenuPid)
            {
                startMenuWindow = window;
                break;
            }
        }

        if (startMenuWindow != NULL)
        {
            startMenuWindow->Hidden = true;
        }
    }
    if (startMenuWindow != NULL)
    {
        if (!startMenuWindow->Hidden && startMenuWindow != activeWindow)
        {
            startMenuWindow->Hidden = true;
        }
    }

    bool doUpdate = false;
    int msgCount = min(msgGetCount(), 50);
    for (int i = 0; i < msgCount; i++)
    {
        GenericMessagePacket* msg = msgGetMessage();
        if (msg == NULL)
            break;
        if (msg->Type == MessagePacketType::MOUSE_EVENT)
        {
            if (msg->Size == sizeof(MouseMessagePacket) && msg->Data != NULL)
            {
                MouseMessagePacket* mouseMsg = (MouseMessagePacket*)msg->Data;
                MousePosition.x = mouseMsg->MouseX;
                MousePosition.y = mouseMsg->MouseY;
                
                bool handled = HandleMouseClickPacket(mouseMsg);

                if (mouseMsg->Type == MouseMessagePacketType::MOUSE_CLICK && !handled)
                {   
                    if (activeWindow != NULL)
                    {
                        GenericMessagePacket* msgNew = new GenericMessagePacket(MessagePacketType::MOUSE_EVENT, msg->Data, sizeof(MouseMessagePacket));
                        msgSendConv(msgNew, activeWindow->PID, activeWindow->CONVO_ID_WM_MOUSE_STUFF);
                        msgNew->Free();
                        _Free(msgNew);
                    }
                }

                if (handled && activeWindow != NULL)
                {
                    windowsUpdated->AddIfUnique(activeWindow);
                }
            }
        }
        else if (msg->Type == MessagePacketType::KEY_EVENT)
        {
            if (msg->Size == sizeof(KeyMessagePacket) && msg->Data != NULL)
            {
                KeyMessagePacket* keyMsg = (KeyMessagePacket*)msg->Data;
                if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED && keyMsg->Scancode == 0x58) // F12
                {
                    uint64_t newPid = startProcess("bruh:programs/shell/shell.elf", 0, NULL, "");
                }
                else if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED && keyMsg->Scancode == 0x3F) // F5 
                {
                    for (int i = 0; i < 10; i++)
                        uint64_t newPid = startProcess("bruh:programs/empty/empty.elf", 0, NULL, "");
                }
                else if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED && keyMsg->Scancode == 0x40) // F6
                {
                    uint64_t newPid = startProcess("bruh:programs/miniWinTest/miniWinTest.elf", 0, NULL, "");
                }
                else if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED && keyMsg->Scancode == 0x57) // F11
                {
                    Clear(true);
                    UpdatePointerRect(0, 0, actualScreenFramebuffer->Width - 1, actualScreenFramebuffer->Height - 1);
                    RenderWindows();
                    RenderActualSquare(0, 0, actualScreenFramebuffer->Width - 1, actualScreenFramebuffer->Height - 1);
                }
                else if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED && keyMsg->Scancode == 0x44) // F10
                {
                    uint64_t newPid = startProcess("bruh:programs/explorer/explorer.elf", 0, NULL, "");
                }
                else if (activeWindow != NULL)
                {
                    GenericMessagePacket* msgNew = new GenericMessagePacket(MessagePacketType::KEY_EVENT, msg->Data, sizeof(KeyMessagePacket));
                    msgSendConv(msgNew, activeWindow->PID, activeWindow->CONVO_ID_WM_KB_STUFF);
                    msgNew->Free();
                    _Free(msgNew);
                }

                // if (keyMsg->Type == KeyMessagePacketType::KEY_PRESSED)
                // {
                //     actualScreenRenderer->CursorPosition.x = 0;
                //     actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 128;

                //     actualScreenRenderer->Clear(
                //         0, actualScreenRenderer->CursorPosition.y, 
                //         160, actualScreenRenderer->CursorPosition.y + 16, 
                //         Colors.black
                //     );

                //     actualScreenRenderer->Println("> KEY {} HELD", to_string((int)keyMsg->Scancode), Colors.white);
                // }                
                // else if (keyMsg->Type == KeyMessagePacketType::KEY_RELEASE)
                // {
                //     actualScreenRenderer->CursorPosition.x = 0;
                //     actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 128;

                //     actualScreenRenderer->Clear(
                //         0, actualScreenRenderer->CursorPosition.y, 
                //         160, actualScreenRenderer->CursorPosition.y + 16, 
                //         Colors.black
                //     );

                //     actualScreenRenderer->Println("> KEY {} RELEASED", to_string((int)keyMsg->Scancode), Colors.white);
                // }
            }
        }
        else
        {
            tempPackets->Add(msg);
            continue;
        }

        msg->Free();
        _Free(msg);
    }

    for (int i = 0; i < tempPackets->GetCount(); i++)
    {
        GenericMessagePacket* msg = tempPackets->ElementAt(i);

        if (msg->Type == MessagePacketType::WINDOW_BUFFER_EVENT)
        {
            WindowBufferUpdatePacket* windowBufferUpdatePacket = new WindowBufferUpdatePacket(msg);

            updateFramePackets->Enqueue(windowBufferUpdatePacket);

            // some goofy code bc we using references
            msg = new GenericMessagePacket(MessagePacketType::WINDOW_BUFFER_EVENT, NULL, 0);
        }
        else if (msg->Type == MessagePacketType::WINDOW_CREATE_EVENT && (msg->Size == 0 || msg->Size == 8))
        {
            uint64_t pidFrom = msg->FromPID;
            uint64_t newWindowId = 0;
            if (msg->Size == 8)
                newWindowId = *(uint64_t*)msg->Data;
                
            if (newWindowId == 0)
                newWindowId = RND::RandomInt();

            Window* window = new Window(100 + RND::RandomInt() % 100, 100 + RND::RandomInt() % 100, 400, 300, "Window", newWindowId, pidFrom);
            windows->Add(window);
            _memset(window->Buffer->BaseAddress, 0x20, window->Buffer->BufferSize);

            activeWindow = window;

            //Clear(true);
            //RenderWindows();
            totalPixelCount += ActuallyRenderWindow(window, false);
            ScreenUpdates->Enqueue(WindowUpdate(
                window->Dimensions.x - 1,
                window->Dimensions.y - 24,
                window->Dimensions.x + window->Dimensions.width + 1,
                window->Dimensions.y + window->Dimensions.height + 1
                ));
            
            // Load the window icon if it exists
            {
                const char* elfPath = getElfPath(pidFrom);
                if (elfPath != NULL)
                {
                    const char* elfDrive = FS_EXTRA::GetDriveNameFromFullPath(elfPath);
                    if (elfDrive != NULL)
                    {
                        const char* elfFolder = FS_EXTRA::GetFolderPathFromFullPath(elfPath);
                        if (elfFolder != NULL)
                        {
                            elfDrive = StrCombineAndFree(elfDrive, ":");
                            elfFolder = StrCombineAndFree(elfFolder, "/assets/icon.mbif");
                            const char* res = StrCombine(elfDrive, elfFolder);

                            serialPrint("IMG FILE: ");
                            serialPrintLn(res);

                            void* buffer = NULL;
                            uint64_t byteCount = 0;
                            if (fsReadFile(res, &buffer, &byteCount))
                            {
                                ImageStuff::BitmapImage* img = ImageStuff::ConvertBufferToBitmapImage((char*)buffer, byteCount);
                                if (img != NULL)
                                {
                                    WindowIconEntry* entry = new WindowIconEntry(window, img);
                                    windowIconEntries->Add(entry);
                                }
                                serialPrintLn("File does exist!");
                            }
                            else
                                serialPrintLn("File does not exist");
                            

                            if (buffer != NULL)
                                _Free(buffer);
                            _Free(res);
                            _Free(elfFolder);
                        }

                        _Free(elfDrive);
                    }
                }
                else
                    serialPrintLn("NULL");
                _Free(elfPath);
            }

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
                msgRespondConv(msg, response);
                response->Free();
                _Free(response);
            }
        }
        else if (msg->Type == MessagePacketType::WINDOW_DELETE_EVENT && msg->Size == 8)
        {
            uint64_t pidFrom = msg->FromPID;
            uint64_t wantedWindowId = *(uint64_t*)msg->Data;

            Window* window = NULL;
            for (int i = 0; i < windows->GetCount(); i++)
            {
                Window* tWin = windows->ElementAt(i);
                if (tWin->ID == wantedWindowId)
                {
                    window = tWin;
                    break;
                }
            }

            if (window != NULL)
            {
                if (window->PID == pidFrom)
                {
                    {
                        GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::WINDOW_DELETE_EVENT, NULL, 0);
                        msgSendConv(packet, window->PID, window->CONVO_ID_WM_WINDOW_CLOSED);
                        packet->Free();
                        _Free(packet);
                    }
                    AddWindowToBeRemoved(window);
                }
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
                    //msgSendMessage(sendMsg, msg->FromPID);
                    msgRespondConv(msg, sendMsg);
                    
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
                    win->UpdateUsingPartialWindow(fromWind, true, false, false);
                    //win->UpdateCheck();
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

    tempPackets->Clear();

    //doUpdate |= updateFramePackets->GetCount() > 0;

    // check for window updates
    for (int i = 0; i < windows->GetCount(); i++)
    {
        Window* window = windows->ElementAt(i);

        {
            window->CurrentTitleBackgroundColor = window->DefaultTitleBackgroundColor;
            bool oldActive = window->IsActive;
            window->IsActive = (window == activeWindow);
            if (oldActive != window->IsActive)
                windowsUpdated->AddIfUnique(window);
            if (window == activeWindow)
            {
                window->CurrentTitleColor = window->SelectedTitleColor;
                window->CurrentBorderColor = window->SelectedBorderColor;
            }
            else
            {
                window->CurrentTitleColor = window->DefaultTitleColor;
                window->CurrentBorderColor = window->DefaultBorderColor;
            }
        }

        window->UpdateCheck();

        if (window->Updates->GetCount() > 0)
        {
            doUpdate = true;

            for (int j = 0; j < window->Updates->GetCount(); j++)
            {
                WindowUpdate update = window->Updates->ElementAt(j);

                if (update.outsideWindow)
                    UpdatePointerRect(
                        window->Dimensions.x + update.x1, 
                        window->Dimensions.y + update.y1, 
                        
                        window->Dimensions.x + update.x2, 
                        window->Dimensions.y + update.y2
                    );

                ScreenUpdates->Enqueue(WindowUpdate(
                    window->Dimensions.x + update.x1, 
                    window->Dimensions.y + update.y1, 
                    
                    window->Dimensions.x + update.x2, 
                    window->Dimensions.y + update.y2
                ));
            }

            window->Updates->Clear();

            if (windowsUpdated->Contains(window))
            {
                WindowObjectPacket* winObjPacketTo = new WindowObjectPacket(window, false);
                GenericMessagePacket* sendMsg = winObjPacketTo->ToGenericMessagePacket();

                msgSendConv(sendMsg, window->PID, window->CONVO_ID_WM_WINDOW_UPDATE);
                //programWait(100);

                sendMsg->Free();
                _Free(sendMsg);

                winObjPacketTo->Free();
                _Free(winObjPacketTo);
            }
        }

        
    }


    for (int i = 0; i < windowsToDelete->GetCount(); i++)
    {
        Window* window = windowsToDelete->ElementAt(i);

        UpdatePointerRect(
            window->Dimensions.x - 1,
            window->Dimensions.y - 24,
            window->Dimensions.x + window->Dimensions.width + 1,
            window->Dimensions.y + window->Dimensions.height + 1
        );

        ScreenUpdates->Enqueue(WindowUpdate(
            window->Dimensions.x - 1,
            window->Dimensions.y - 24,
            window->Dimensions.x + window->Dimensions.width + 1,
            window->Dimensions.y + window->Dimensions.height + 1
            ));

        if (activeWindow == window)
            activeWindow = NULL;

        window->Free();
        _Free(window);
    }
    windowsToDelete->Clear();


    doUpdate = updateFramePackets->GetCount() != 0;

    doUpdate |= MousePosition != oldMousePos;

    doUpdate |= ScreenUpdates->GetCount() != 0;

    uint64_t currTime = envGetTimeMs();
    if (lastFrameTime + 500 < currTime)
    {
        lastFrameTime = currTime;
        doUpdate = true;
        Taskbar::Scounter = 10000;
    }

    if (!doUpdate)
        return 0;

    bool taskbarRendered = Taskbar::RenderTaskbar();

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

            ScreenUpdates->Enqueue(WindowUpdate(
                win->Dimensions.x + packet->X, 
                win->Dimensions.y + packet->Y, 
                
                win->Dimensions.x + packet->X + packet->Width - 1, 
                win->Dimensions.y + packet->Y + packet->Height - 1
            ));
        }

        
        packet->Free();
        _Free(packet);
    }



    
    if (taskbarRendered)
        totalPixelCount += RenderActualSquare(
            0, 
            mainBuffer->Height - taskbar->Height, 
            
            mainBuffer->Width - 1, 
            mainBuffer->Height - 1
        );



    // Draw Mouse
    MPoint tempMousePos = MousePosition;
    DrawMousePointerNew(tempMousePos, pointerBuffer);
    
    totalPixelCount += RenderActualSquare(
        oldMousePos.x - 32, 
        oldMousePos.y - 32, 
        
        oldMousePos.x + 48, 
        oldMousePos.y + 48
    );

    totalPixelCount += RenderActualSquare(
        tempMousePos.x - 32, 
        tempMousePos.y - 32, 
        
        tempMousePos.x + 48, 
        tempMousePos.y + 48
    );
    oldMousePos = tempMousePos;

    int pixelSum = 0;
    int maxPixelsPerFrame = 10000;
    
    while (ScreenUpdates->GetCount() > 0)
    {
        WindowUpdate update = ScreenUpdates->Dequeue();

        pixelSum += RenderActualSquare(
            update.x1, 
            update.y1, 
            
            update.x2, 
            update.y2
        );

        totalPixelCount += pixelSum;

        if ((pixelSum > maxPixelsPerFrame) && ScreenUpdates->GetCount() < 5)
            break;
    }
    //serialPrint("UPDATES LEFT: ");
    //serialPrintLn(to_string(ScreenUpdates->GetCount()));
    //ScreenUpdates->Clear();



    // Remove Mouse
    UpdatePointerRect(tempMousePos.x - 32, tempMousePos.y - 32, tempMousePos.x + 64, tempMousePos.y + 64);



    //actualScreenRenderer->Clear(300, 300, 320, 320, rndCol);

    return totalPixelCount;
}

#include <libm/math.h>

Window* getWindowAtMousePosition(int dis)
{
    if (MousePosition.y >= mainBuffer->Height - taskbar->Height)
        return NULL;
    
    //GlobalRenderer->Println("Mouse POS Check");
    for (int64_t i = windows->GetCount() - 1; i >= 0; i--)
    {
        if (windows->ElementAt(i)->Hidden)
            continue;

        WindowDimension dim = windows->ElementAt(i)->Dimensions;
        MPoint tl = MPoint(dim.x, dim.y);
        MPoint br = MPoint(dim.x + dim.width, dim.y + dim.height);
        
        tl.x--;
        tl.y--;
        if (windows->ElementAt(i)->ShowTitleBar)
            tl.y -= 21;
        br.x++;
        br.y++;

        tl.x -= dis;
        tl.y -= dis;
        br.x += dis;
        br.y += dis;


        if (MousePosition.x >= tl.x && MousePosition.x <= br.x && MousePosition.y >= tl.y && MousePosition.y <=br.y)
            return windows->ElementAt(i);
    }
    return NULL;   
}

Window* getWindowAtMousePosition()
{
    return getWindowAtMousePosition(8);
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



