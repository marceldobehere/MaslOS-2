#include "mouse.h"
#include "../../kernelStuff/IO/IO.h"
#include "../../osData/MStack/MStackM.h"
#include "../pit/pit.h"

#define PS2XSign        0b00010000
#define PS2YSign        0b00100000
#define PS2XOverflow    0b01000000
#define PS2YOverflow    0b10000000

#define PS2LeftButton   0b00000001
#define PS2MiddleButton 0b00000100
#define PS2RightButton  0b00000010

#define MouseLeft   0
#define MouseRight  1 
#define MouseMiddle 2

namespace Mouse
{
    //bool clicks[3] = {false, false, false};
    //uint64_t clickTimes[3] = {0, 0, 0};
    Lockable<Queue<MousePacket>*> mousePackets;

    // MPoint IMousePosition;
    // MPoint MousePosition;
    bool ShowMouseCursor = true;



    // void SetMousePosition(MPoint point)
    // {
    //     IMousePosition = point;
    //     MousePosition = point;
    // }

    // void FigureOutCorrectMouseImage()
    // {
    //     int maxDis = 8;
    //     for (int i = 0; i < 4; i++)
    //         dragArr[i] = false;
    //     Window* window = WindowManager::getWindowAtMousePosition(maxDis);
    //     if (window == NULL || !window->resizeable)
    //     {
    //         currentMouseImageName = "default.mbif";
            
    //         return;
    //     }
        
    //     int minY = 22;
    //     if (!window->showTitleBar)
    //         minY = 1;

    //     if (IMousePosition.x >= window->position.x - maxDis && IMousePosition.x <= window->position.x + maxDis)
    //     {
    //         dragArr[0] = true;
    //         if (IMousePosition.y >= (window->position.y - minY) - maxDis && IMousePosition.y <= (window->position.y - minY) + maxDis)
    //         {
    //             currentMouseImageName = "drag_D_d.mbif";
    //             dragArr[1] = true;

    //         }
    //         else if (IMousePosition.y >= (window->position.y + window->size.height) - maxDis && IMousePosition.y <= (window->position.y + window->size.height) + maxDis)
    //         {
    //             currentMouseImageName = "drag_U_d.mbif";
    //             dragArr[3] = true;
    //         }
    //         else 
    //         {
    //             currentMouseImageName = "drag_x.mbif";
    //         }
    //     }
    //     else if (IMousePosition.x >= (window->position.x + window->size.width) - maxDis && IMousePosition.x <= (window->position.x + window->size.width) + maxDis)
    //     {
    //         dragArr[2] = true;
    //         if (IMousePosition.y >= (window->position.y - minY) - maxDis && IMousePosition.y <= (window->position.y - minY) + maxDis)
    //         {
    //             currentMouseImageName = "drag_U_d.mbif";
    //             dragArr[1] = true;
    //         }
    //         else if (IMousePosition.y >= (window->position.y + window->size.height) - maxDis && IMousePosition.y <= (window->position.y + window->size.height) + maxDis)
    //         {
    //             currentMouseImageName = "drag_D_d.mbif";
    //             dragArr[3] = true;
    //         }
    //         else
    //         {
    //             currentMouseImageName = "drag_x.mbif";
    //         }
    //     }
    //     else if (IMousePosition.y >= (window->position.y - minY) - maxDis && IMousePosition.y <= (window->position.y - minY) + maxDis)
    //     {
    //         currentMouseImageName = "drag_y.mbif";
    //         dragArr[1] = true;
    //     }
    //     else if (IMousePosition.y >= (window->position.y + window->size.height) - maxDis && IMousePosition.y <= (window->position.y + window->size.height) + maxDis)
    //     {
    //         currentMouseImageName = "drag_y.mbif";
    //         dragArr[3] = true;
    //     }
    //     else
    //     {
    //         currentMouseImageName = "default.mbif";
    //     }
    // }

    // void DrawMousePointerNew(MPoint point, PointerFramebuffer* framebuffer)
    // {
    //     if (&osData.windows != NULL)
    //     {
    //         FigureOutCorrectMouseImage();
    //     }
    //     if (mouseZIP != NULL)
    //     {
    //         if (!StrEquals(oldMouseImageName, currentMouseImageName))
    //         {
    //             oldMouseImageName = currentMouseImageName;
    //             kernelFiles::ImageFile* oldMouseImage = currentMouseImage;
    //             currentMouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(mouseZIP, currentMouseImageName));
                
    //             if (oldMouseImage != NULL)
    //             {
    //                 _Free(oldMouseImage->imageBuffer);
    //                 _Free(oldMouseImage);
    //             }
    //         }
    //     }

    //     if (currentMouseImage != NULL)
    //         VirtualRenderer::DrawImage(currentMouseImage, point.x, point.y, 1, 1, VirtualRenderer::Border(framebuffer), framebuffer);
    //     else
    //         ;//DrawMouseBuffer(IMousePosition, framebuffer);
    // }

    // void DrawMousePointer1(PointerFramebuffer* framebuffer)
    // {
    //     //LoadFromBuffer(oldIMousePosition, framebuffer);
    // }

    // void DrawMousePointer2(PointerFramebuffer* framebuffer, MPoint mousePos)
    // {
    //     //SaveIntoBuffer(IMousePosition, framebuffer);
    //     //DrawMouseBuffer(IMousePosition, framebuffer);
    //     if (ShowMouseCursor)
    //         DrawMousePointerNew(mousePos, framebuffer);
    //     MousePosition = IMousePosition;
    // }



    void Mousewait()
    {
        uint64_t timeout = 1000000;
        while (timeout--)
            if ((inb(0x64) & 0b10) == 0)
                return;

    }

    void MousewaitInput()
    {
        uint64_t timeout = 1000000;
        while (timeout--)
            if (inb(0x64) & 0b1)
                return;

    }

    void MouseWrite(uint8_t value)
    {
        Mousewait();
        outb(0x64, 0xD4);
        Mousewait();
        outb(0x60, value);
    }

    uint8_t MouseRead()
    {
        MousewaitInput();
        return inb(0x60);
    }

    int mouseCycleSkip = 0;
    uint8_t MouseCycle = 0;

    MPoint diff = MPoint();
    //bool startDrag = false;
    //Window* dragWindow = NULL;

    void InitPS2Mouse()//(kernelFiles::ZIPFile* _mouseZIP, const char* _mouseName)
    {
        // mouseZIP = _mouseZIP;
        // oldMouseImageName = "";
        // currentMouseImageName = _mouseName;
        
        // for (int i = 0; i < 4; i++)
        //     dragArr[i] = false;

        mouseCycleSkip = 0;
        MouseCycle = 0;


        

        outb(0x64, 0xA8);
        Mousewait();


        mousePackets = Lockable<Queue<MousePacket>*>(new Queue<MousePacket>(4));

        // for (int i = 0; i < 3; i++)
        // {
        //     clicks[i] = false;
        //     clickTimes[i] = PIT::TimeSinceBootMS();
        // }

       
        

        diff = MPoint();
        //startDrag = false;
        //dragWindow = NULL;

        // IMousePosition.x = 0;
        // IMousePosition.y = 0;
        // MousePosition.x = 0;
        // MousePosition.y = 0;
        // currentMouseImage = NULL;
        // //SaveIntoBuffer(IMousePosition, GlobalRenderer->framebuffer);
        // DrawMousePointer();


        outb(0x64, 0x20);
        MousewaitInput();

        uint8_t status = inb(0x60);
        status |= 0b10;
        Mousewait();
        outb(0x64, 0x60);
        Mousewait();
        outb(0x60, status);

        MouseWrite(0xF6);
        MouseRead();
        //Mousewait();


        MouseWrite(0xF4);
        MouseRead();
        //Mousewait();
    }


    uint8_t mousePacketArr[4] = {0, 0, 0, 0};

    #include "../../rnd/rnd.h"

    void HandlePS2Mouse(uint8_t data)
    {
        AddToStack();
        if (mouseCycleSkip != 0)
        {
            MouseCycle = mouseCycleSkip;
            mouseCycleSkip = 0;
            RemoveFromStack();
            return;
        }

        switch(MouseCycle)
        {
            case 0:
            {
                if (data & 0b00001000 == 0)
                {
                    mouseCycleSkip = (RND::lehmer64() / 100) % 4;
                    break;
                }

                mousePacketArr[0] = data;
                MouseCycle++;
                break;
            }
            case 1:
            {
                mousePacketArr[1] = data;
                MouseCycle++;
                break;
            }
            case 2:
            {
                mousePacketArr[2] = data;
                mousePacketArr[3] = 0;
                MouseCycle = 0;

                AddToStack();
                if (!mousePackets.IsLocked())
                {
                    mousePackets.Lock();
                    mousePackets.obj->Enqueue(MousePacket(mousePacketArr));
                    mousePackets.Unlock();
                }
                
                RemoveFromStack();

                break;
            }
            default:
            {
                MouseCycle = 0;
                break;
            }
        }
        
        RemoveFromStack();
    }

    bool activeDrag[4] = {false, false, false, false};
    bool activeDragOn = false;

    // void HandleClick(bool L, bool R, bool M)
    // {
    //     AddToStack();
    
    //     activeDragOn = false;
    //     for (int i = 0; i < 4; i++)
    //         activeDrag[i] = 0;

    //     //activeWindow->renderer->Println("Click");
    //     if (L || R || M)
    //     {
    //         AddToStack();
    //         Window* oldActive = activeWindow;
    //         Window* window = WindowManager::getWindowAtMousePosition();
    //         RemoveFromStack();

    //         AddToStack();
    //         WindowActionEnum action = WindowActionEnum::_NONE;
    //         if (L && WindowManager::currentActionWindow != NULL)
    //         {
    //             if (WindowManager::currentActionWindow == window)
    //             {
    //                 action = WindowManager::currentActionWindow->GetCurrentAction();
    //                 if (action == WindowActionEnum::_NONE)
    //                     WindowManager::currentActionWindow = NULL;
    //             }
    //         }
    //         RemoveFromStack();

    //         AddToStack();
    //         if (action != WindowActionEnum::_NONE) // Window Button Clicked
    //         {
    //             AddToStack();
    //             if (action == WindowActionEnum::CLOSE)
    //             {
    //                 AddToStack();
    //                 if (osData.debugTerminalWindow != WindowManager::currentActionWindow)
    //                 {
    //                     // osData.debugTerminalWindow->Log("Count: {}", to_string(osData.osTasks.getCount()), Colors.yellow);
    //                     // osData.debugTerminalWindow->Log("Cap: {}", to_string(osData.osTasks.getCapacity()), Colors.yellow);
    //                     osData.osTasks.Add(NewWindowCloseTask(WindowManager::currentActionWindow));
    //                 }
    //                 else
    //                 {
    //                     AddToStack();
    //                     osData.showDebugterminal = false;
    //                     osData.windowPointerThing->UpdatePointerRect(
    //                         osData.debugTerminalWindow->position.x - 1, 
    //                         osData.debugTerminalWindow->position.y - 23, 
    //                         osData.debugTerminalWindow->position.x + osData.debugTerminalWindow->size.width, 
    //                         osData.debugTerminalWindow->position.y + osData.debugTerminalWindow->size.height
    //                         );
    //                     RemoveFromStack();
    //                 }
    //                 RemoveFromStack();
    //             }
    //             else if (action == WindowActionEnum::HIDE)
    //             {
    //                 AddToStack();
    //                 WindowManager::currentActionWindow->hidden = true;
    //                 if (activeWindow == WindowManager::currentActionWindow)
    //                     activeWindow = NULL;
    //                 RemoveFromStack();
    //             }
    //             else if (action == WindowActionEnum::MIN_MAX)
    //             {
    //                 AddToStack();
                    
    //                 osData.windowsToGetActive.Enqueue(WindowManager::currentActionWindow);
    //                 if (WindowManager::currentActionWindow->resizeable)
    //                     WindowManager::currentActionWindow->maximize = !WindowManager::currentActionWindow->maximize;
    //                 RemoveFromStack();
    //             }
    //             RemoveFromStack();
    //         }
    //         else
    //         {
    //             AddToStack();
    //             oldActive = activeWindow;
    //             //if (L)
    //             {
    //                 activeWindow = window;
                    
    //                 dragWindow = window;
    //             }
    //             RemoveFromStack();

    //             startDrag = false;
    //             if (Taskbar::activeTabWindow != NULL || (L && Taskbar::MButtonSelected))
    //             {
    //                 AddToStack();
    //                 if (L && Taskbar::activeTabWindow != NULL) // Taskbar Button Clicked
    //                 {
    //                     activeWindow = Taskbar::activeTabWindow;
    //                     Taskbar::activeTabWindow->moveToFront = true;
    //                     activeWindow->hidden = false;
    //                 }
    //                 RemoveFromStack();

    //                 AddToStack();
    //                 if (M && Taskbar::activeTabWindow != NULL) // Taskbar Button Middle-Clicked
    //                 {
    //                     osData.osTasks.Add(NewWindowCloseTask(Taskbar::activeTabWindow));
    //                 }
    //                 RemoveFromStack();

    //                 AddToStack();
    //                 if (L && Taskbar::MButtonSelected)
    //                 {
    //                     if (oldActive == osData.startMenuWindow)
    //                     {
    //                         activeWindow = NULL;
    //                     }
    //                     else
    //                     {
    //                         osData.startMenuWindow->hidden = false;
    //                         osData.startMenuWindow->moveToFront = true;
    //                         int sH = osData.startMenuWindow->size.height;
    //                         osData.startMenuWindow->newPosition.y = (osData.windowPointerThing->actualScreenBuffer->Height - sH - osData.windowPointerThing->taskbar->Height);
    //                         activeWindow = osData.startMenuWindow;
    //                     }
    //                 }
    //                 RemoveFromStack();
    //             }
    //             else if (MousePosition.y >= osData.windowPointerThing->virtualScreenBuffer->Height - osData.windowPointerThing->taskbar->Height)
    //             {
    //                 osData.windowsToGetActive.Enqueue(NULL);    
    //             }
    //             else if (window != NULL)
    //             {
    //                 AddToStack();
    //                 activeDragOn = false;
    //                 for (int i = 0; i < 4; i++)
    //                 {
    //                     activeDrag[i] = dragArr[i];
    //                     activeDragOn |= activeDrag[i];
    //                 }

    //                 diff.x = MousePosition.x;
    //                 diff.y = MousePosition.y;
    //                 window->moveToFront = true;
    //                 RemoveFromStack();

    //                 AddToStack();
    //                 if (!activeDragOn && MousePosition.y > window->position.y + 0)
    //                 {
    //                     dragWindow = NULL;
    //                     if (window->instance != NULL && window->instance->instanceType == InstanceType::GUI)
    //                     {
    //                         GuiInstance* gui = (GuiInstance*)window->instance;
    //                         // if (gui->screen != NULL && gui->screen->selectedComponent != NULL)
    //                         if (gui->screen != NULL && gui->waitTask == NULL)
    //                         {
    //                             Position p = window->GetMousePosRelativeToWindow();
                                
    //                             if (oldActive == window)
    //                                 gui->screen->MouseClicked(GuiComponentStuff::MouseClickEventInfo(GuiComponentStuff::Position(p.x, p.y), L, R, M));
    //                         }
    //                     }
    //                 }
    //                 RemoveFromStack();
    //                 //osData.windowPointerThing->UpdateWindowBorder(osData.windows[osData.windows.getCount() - 1]);
    //             }
    //             else
    //             {
    //                 //osData.windowsToGetActive.add(NULL);    
    //                 //osData.windowPointerThing->UpdateWindowBorder(osData.windows[osData.windows.getCount() - 1]);


    //             }
    //         }
    //         RemoveFromStack();
            
    //         AddToStack();
    //         if (oldActive != NULL)
    //         {
    //             osData.windowPointerThing->UpdateWindowBorder(oldActive);
    //         }
    //         RemoveFromStack();
    //     }  
    //     RemoveFromStack(); 
    // }


    // void HandleHold(bool L, bool R, bool M)
    // {
    //     AddToStack();
    //     if (L)
    //     {
    //         Window* window = dragWindow;
    //         if (window != NULL)
    //         {
    //             if (!startDrag && window->moveable)
    //             {
    //                 startDrag = true;
    //             }
    //             else if (startDrag)
    //             {
    //                 if (!activeDragOn)
    //                 {
    //                     window->newPosition.x += (MousePosition.x - diff.x);
    //                     window->newPosition.y += (MousePosition.y - diff.y);
    //                 }
    //                 else
    //                 {
    //                     if (activeDrag[0])
    //                     {
    //                         int32_t tempDiff = (MousePosition.x - diff.x); 
    //                         if (window->newSize.width - tempDiff < 80)
    //                             tempDiff = window->newSize.width - 80;

    //                         window->newSize.width -= tempDiff;
    //                         window->newPosition.x += tempDiff;
    //                     }
    //                     if (activeDrag[1])
    //                     {
    //                         int32_t tempDiff = (MousePosition.y - diff.y); 
    //                         if (window->newSize.height - tempDiff < 20)
    //                             tempDiff = window->newSize.height - 20;

    //                         window->newSize.height -= tempDiff;
    //                         window->newPosition.y += tempDiff;
    //                     }
    //                     if (activeDrag[2])
    //                     {
    //                         window->newSize.width += (MousePosition.x - diff.x);
    //                     }
    //                     if (activeDrag[3])
    //                     {
    //                         window->newSize.height += (MousePosition.y - diff.y);
    //                     }
    //                 }

    //                 diff.x = MousePosition.x;
    //                 diff.y = MousePosition.y;
    //             }
    //         }
    //     }
    //     RemoveFromStack();
    // }



    void ProcessMousePackets()
    {
        //mousePackets->clear();
        ProcessMousePackets(20);
    }

    void ProcessMousePackets(int limit)
    {
        AddToStack();
        if (mousePackets.IsLocked())
            return;
        
        mousePackets.Lock();
        
        
        int l = mousePackets.obj->GetCount();
        if (l > limit)
            l = limit;
        for (int i = 0; i < l; i++)
            ProcessMousePacket(mousePackets.obj->Dequeue());
        
        mousePackets.Unlock();
        RemoveFromStack();
    }

    int MousePacketsAvailable()
    {
        int count = 0;
        if (!mousePackets.IsLocked())
        {
            mousePackets.Lock();
            count = mousePackets.obj->GetCount();
            mousePackets.Unlock();
        }
        return count;
    }

    MiniMousePacket ProcessMousePacket(MousePacket packet)
    {
        AddToStack();

        bool xNegative, yNegative, xOverflow, yOverflow, leftButton, middleButton, rightButton;

        if(packet.data[0] & PS2XSign)
            xNegative = true;
        else
            xNegative = false;

        if(packet.data[0] & PS2YSign)
            yNegative = true;
        else
            yNegative = false;

        if(packet.data[0] & PS2XOverflow)
            xOverflow = true;
        else
            xOverflow = false;

        if(packet.data[0] & PS2YOverflow)
            yOverflow = true;
        else
            yOverflow = false;


        if(packet.data[0] & PS2LeftButton)
            leftButton = true;
        else
            leftButton = false;

        if(packet.data[0] & PS2RightButton)
            rightButton = true;
        else
            rightButton = false;

        if(packet.data[0] & PS2MiddleButton)
            middleButton = true;
        else
            middleButton = false;

        MPoint tempM = MPoint();
        tempM.x = 0;
        tempM.y = 0;


        if (!xNegative)
        {
            if (true)//(packet.data[1] < 200 || xOverflow)
            {
                tempM.x += packet.data[1];
                if (xOverflow)
                {
                    mouseCycleSkip = (PIT::TicksSinceBoot) % 4;
                    RemoveFromStack();
                    return InvalidMousePacket;
                }
                    ;//IMousePosition.x += 255;
            }
            else
            {
                mouseCycleSkip = 1;
                RemoveFromStack();
                return InvalidMousePacket;
            }
        }
        else
        {
            packet.data[1] = 256 - packet.data[1];
            
            if (true)//(packet.data[1] < 200 || xOverflow)
            {
                tempM.x -= packet.data[1];
                if (xOverflow)
                {
                    mouseCycleSkip = (PIT::TicksSinceBoot) % 4;
                    RemoveFromStack();
                    return InvalidMousePacket;
                }
                    ;//IMousePosition.x -= 255;   
            }
            else
            {
                mouseCycleSkip = 1;
                RemoveFromStack();
                return InvalidMousePacket;
            }
            
        }

        if (yNegative)
        {
            packet.data[2] = 256 - packet.data[2];

            if (true)//(packet.data[2] < 200 || yOverflow)
            {
                tempM.y += packet.data[2];
                if (yOverflow)
                {
                    mouseCycleSkip = (PIT::TicksSinceBoot ) % 4;
                    RemoveFromStack();
                    return InvalidMousePacket;
                }
                    ;//IMousePosition.y += 255;
            }
            else
            {
                mouseCycleSkip = 1;
                RemoveFromStack();
                return InvalidMousePacket;
            }
        }
        else
        {
            //MousePacket[2] = 256 - MousePacket[2];
            if (true)//(packet.data[2] < 200 || yOverflow)
            {
                tempM.y -= packet.data[2];
                if (yOverflow)
                {
                    mouseCycleSkip = (PIT::TicksSinceBoot) % 4;
                    RemoveFromStack();
                    return InvalidMousePacket;
                }
                    ;//IMousePosition.y -= 255;
            }
            else
            {
                mouseCycleSkip = 1;
                RemoveFromStack();
                return InvalidMousePacket;
            }
        }

        

        //GlobalRenderer->overwrite = true;

        // GlobalRenderer->CursorPosition.x = 0;
        // GlobalRenderer->CursorPosition.y = 32;
        // GlobalRenderer->Println("X: {}        ", to_string((int64_t)IMousePosition.x));
        // GlobalRenderer->Println("Y: {}        ", to_string((int64_t)IMousePosition.y));

        tempM.x = (tempM.x * osData.mouseSensitivity) / 100;
        tempM.y = (tempM.y * osData.mouseSensitivity) / 100;

        //IMousePosition.x += tempM.x;
        //IMousePosition.y += tempM.y;
        


        // if(IMousePosition.x < 0)
        //     IMousePosition.x = 0;
        // else if(IMousePosition.x > GlobalRenderer->framebuffer->Width - 16)
        //     IMousePosition.x = GlobalRenderer->framebuffer->Width - 16;

        // if(IMousePosition.y < 0)
        //     IMousePosition.y = 0;
        // else if(IMousePosition.y > GlobalRenderer->framebuffer->Height - 16)
        //     IMousePosition.y = GlobalRenderer->framebuffer->Height - 16;


        //DrawMousePointer();

        // {
        //     LoadFromBuffer(oldIMousePosition);
        //     //if (leftButton)
        //     //    GlobalRenderer->delChar(IMousePosition.x, IMousePosition.y, mouseColFront);
        //     //if (rightButton)
        //     //    GlobalRenderer->delChar(IMousePosition.x, IMousePosition.y, mouseColBack);
        //     SaveIntoBuffer(IMousePosition);
        //     DrawMouseBuffer(IMousePosition);
        //     oldIMousePosition.x = IMousePosition.x;
        //     oldIMousePosition.y = IMousePosition.y;
        // }

        bool cClicks[3] = {leftButton, rightButton, middleButton};
        // MouseClickState[0] = leftButton;
        // MouseClickState[1] = rightButton;
        // MouseClickState[2] = middleButton;

        // bool tClicks[3] = {false, false, false};
        // bool tHolds[3] = {false, false, false};
        // uint64_t time = PIT::TimeSinceBootMS();
        // for (int i = 0; i < 3; i++)
        // {
        //     if (!cClicks[i])
        //     {
        //         if (clicks[i])
        //         {  
        //             clicks[i] = false; 
        //         }
        //     }
        //     else
        //     {
        //         if (clicks[i])
        //         {   
        //             //if (time >= clickTimes[i] + 50)
        //                 tHolds[i] = true;  
        //         }
        //         else
        //         {
        //             clicks[i] = true;
        //             tClicks[i] = true;
        //             clickTimes[i] = time;
        //         }
        //     }
        // }

        MiniMousePacket res;

        res.Valid = true;
        res.X = tempM.x;
        res.Y = tempM.y;
        res.LeftButton = leftButton;
        res.RightButton = rightButton;
        res.MiddleButton = middleButton;

        //MousePosition = IMousePosition;
        // clicks[0] = leftButton;
        // clicks[1] = rightButton;
        // clicks[2] = middleButton;

        // if(tClicks[0] || tClicks[1] || tClicks[2])
        //     HandleClick(tClicks[0], tClicks[1], tClicks[2]);

        // if(tHolds[0] || tHolds[1] || tHolds[2])
        //     HandleHold(tHolds[0], tHolds[1], tHolds[2]);

        RemoveFromStack();
        return res;
    }

}