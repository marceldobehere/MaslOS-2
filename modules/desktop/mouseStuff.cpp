#include "mouseStuff.h"
#include <libm/rendering/Cols.h>
#include "main.h"

bool DrawMouse = true;

uint32_t MouseDataMap[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 14738687, 11714047, 11714047, 8163583, 8163583, 8163583, 4020991, 2508543, 2508543, 1851135, 1851135, 9983, 0, 0, 0, 
    0, 11714047, 11714047, 8163583, 8163583, 4020991, 2508543, 2508543, 2508543, 2508543, 1851135, 9983, 0, 0, 0, 0,
    0, 11714047, 8163583, 4020991, 4020991, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 0, 0, 0, 0, 0,
    0, 8163583, 8163583, 4020991, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 0, 0, 0, 0, 0, 0,
    0, 8163583, 4020991, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 0, 0, 0, 0, 0, 0, 0,
    0, 8163583, 2508543, 2508543, 2508543, 1851135, 1851135, 1851135, 470271, 470271, 0, 0, 0, 0, 0, 0,
    0, 4020991, 2508543, 1851135, 1851135, 9983, 470271, 470007, 470007, 9197, 9197, 0, 0, 0, 0, 0,
    0, 2508543, 2508543, 1851135, 470271, 470271, 470271, 470007, 470007, 9197, 9197, 9197, 0, 0, 0, 0,
    0, 1851135, 1851135, 1851135, 470271, 0, 470007, 470007, 9197, 9197, 9197, 8408, 8408, 0, 0, 0,
    0, 470271, 470271, 470271, 0, 0, 0, 9197, 9197, 9197, 8408, 8408, 8408, 6829, 0, 0,
    0, 470271, 470271, 0, 0, 0, 0, 0, 9983, 8408, 8408, 8408, 6829, 6829, 4991, 0,
    0, 470271, 0, 0, 0, 0, 0, 0, 0, 8408, 8408, 6829, 6829, 4991, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6829, 6829, 4991, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4991, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


uint8_t MouseShowBitmap[] =
{
    0b11111111, 0b11111110,
    0b11111111, 0b11111100,
    0b11111111, 0b11111000,
    0b11111111, 0b11110000,
    0b11111111, 0b11100000,
    0b11111111, 0b11000000,
    0b11111111, 0b11100000,
    0b11111111, 0b11110000,
    0b11111111, 0b11111000,
    0b11111111, 0b11111100,
    0b11111011, 0b11111110,
    0b11110001, 0b11111111,
    0b11100000, 0b11111110,
    0b11000000, 0b01111100,
    0b10000000, 0b00111000,
    0b00000000, 0b00010000
};

uint32_t CurrentMouseCol = Colors.orange;

void DrawMousePointer(MPoint point, PointerBuffer* framebuffer)
{
    // for now we will just draw the 16x16 hardcoded mouse image
    
    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 16; x++)
            if ((MouseShowBitmap[y * 2 + (x / 8)] & (0x80 >> (x & 7))) != 0)
                if ((point.y + y) >= 0 && (point.y + y) < framebuffer->Height &&
                    (point.x + x) >= 0 && (point.x + x) < framebuffer->Width)
                    framebuffer->BaseAddress[(point.y + y) * framebuffer->Width + (point.x + x)] = &(MouseDataMap[y * 16 + x]);
        
}

ZipStuff::ZIPFile* mouseZIP;
ImageStuff::BitmapImage* currentMouseImage;
const char* currentMouseImageName;
const char* oldMouseImageName = "";
bool dragArr[4] = {false, false, false, false}; //xl yu xr xd

void FigureOutCorrectMouseImage()
{
    int maxDis = 8;
    for (int i = 0; i < 4; i++)
        dragArr[i] = false;
    Window* window = getWindowAtMousePosition(maxDis);
    if (window == NULL || !window->Resizeable)
    {
        currentMouseImageName = "default.mbif";
        
        return;
    }
    
    int minY = 22;
    if (!window->ShowTitleBar)
        minY = 1;

    if (MousePosition.x >= window->Dimensions.x - maxDis && MousePosition.x <= window->Dimensions.x + maxDis)
    {
        dragArr[0] = true;
        if (MousePosition.y >= (window->Dimensions.y - minY) - maxDis && MousePosition.y <= (window->Dimensions.y - minY) + maxDis)
        {
            currentMouseImageName = "drag_D_d.mbif";
            dragArr[1] = true;

        }
        else if (MousePosition.y >= (window->Dimensions.y + window->Dimensions.height) - maxDis && MousePosition.y <= (window->Dimensions.y + window->Dimensions.height) + maxDis)
        {
            currentMouseImageName = "drag_U_d.mbif";
            dragArr[3] = true;
        }
        else 
        {
            currentMouseImageName = "drag_x.mbif";
        }
    }
    else if (MousePosition.x >= (window->Dimensions.x + window->Dimensions.width) - maxDis && MousePosition.x <= (window->Dimensions.x + window->Dimensions.width) + maxDis)
    {
        dragArr[2] = true;
        if (MousePosition.y >= (window->Dimensions.y - minY) - maxDis && MousePosition.y <= (window->Dimensions.y - minY) + maxDis)
        {
            currentMouseImageName = "drag_U_d.mbif";
            dragArr[1] = true;
        }
        else if (MousePosition.y >= (window->Dimensions.y + window->Dimensions.height) - maxDis && MousePosition.y <= (window->Dimensions.y + window->Dimensions.height) + maxDis)
        {
            currentMouseImageName = "drag_D_d.mbif";
            dragArr[3] = true;
        }
        else
        {
            currentMouseImageName = "drag_x.mbif";
        }
    }
    else if (MousePosition.y >= (window->Dimensions.y - minY) - maxDis && MousePosition.y <= (window->Dimensions.y - minY) + maxDis)
    {
        currentMouseImageName = "drag_y.mbif";
        dragArr[1] = true;
    }
    else if (MousePosition.y >= (window->Dimensions.y + window->Dimensions.height) - maxDis && MousePosition.y <= (window->Dimensions.y + window->Dimensions.height) + maxDis)
    {
        currentMouseImageName = "drag_y.mbif";
        dragArr[3] = true;
    }
    else
    {
        currentMouseImageName = "default.mbif";
    }
}

#include <libm/cstrTools.h>

void DrawMousePointerNew(MPoint point, PointerBuffer* framebuffer)
{
    if (!DrawMouse)
        return;
    
    if (windows != NULL)
    {
        FigureOutCorrectMouseImage();
    }
    if (mouseZIP != NULL)
    {
        if (!StrEquals(oldMouseImageName, currentMouseImageName))
        {
            oldMouseImageName = currentMouseImageName;
            ImageStuff::BitmapImage* oldMouseImage = currentMouseImage;
            currentMouseImage = ImageStuff::ConvertFileToBitmapImage(ZipStuff::ZIP::GetFileFromFileName(mouseZIP, currentMouseImageName));
            
            if (oldMouseImage != NULL)
            {
                _Free(oldMouseImage->imageBuffer);
                _Free(oldMouseImage);
            }
        }
    }
    else
        currentMouseImage = NULL;

    if (currentMouseImage != NULL)
        VirtualRenderer::DrawImage(currentMouseImage, point.x, point.y, 1, 1, VirtualRenderer::Border(framebuffer), framebuffer);
    else
        DrawMousePointer(point, framebuffer);//DrawMouseBuffer(IMousePosition, framebuffer);
}

#include "wmStuff.h"
bool doingWindowDrag = false;
MPoint windowDragMouseStart;
WindowDimension windowDragWindowStart;
bool windowDragMouseArr[4];

bool HandleMouseClickPacket(MouseMessagePacket* packet)
{
    if (packet == NULL)
        return false;

    bool tClicks[3] = {false, false, false};
    bool tHolds[3] = {false, false, false};

    tClicks[0] = packet->Left && !packet->PrevLeft;
    tHolds[0] = packet->Left && packet->PrevLeft;

    tClicks[1] = packet->Right && !packet->PrevRight;
    tHolds[1] = packet->Right && packet->PrevRight;

    tClicks[2] = packet->Middle && !packet->PrevMiddle;
    tHolds[2] = packet->Middle && packet->PrevMiddle;

    // instant reset if mouse ever stopped holding
    doingWindowDrag &= packet->Left;
    doingWindowDrag &= activeWindow != NULL;

    bool res = false;

    if (tClicks[0] || tClicks[1] || tClicks[2])
        res |= HandleClick(tClicks[0], tClicks[1], tClicks[2]);
    if (tHolds[0] || tHolds[1] || tHolds[2])
        res |= HandleDrag(tHolds[0], tHolds[1], tHolds[2]);

    return res;
}

#include "taskbar.h"

void MakeWinActive(Window* oldActive, Window* newActive)
{
    activeWindow = newActive;

    if (activeWindow != NULL)
    {
        int idx = windows->GetIndexOf(activeWindow);
        if (idx != windows->GetCount() - 1)
        {
            if (idx != -1)
                windows->RemoveAt(idx);
            windows->Add(activeWindow);
        }
    }

    if (oldActive != activeWindow && oldActive != NULL)
    {
        UpdatePointerRect(
            oldActive->Dimensions.x, 
            oldActive->Dimensions.y, 
            oldActive->Dimensions.x + oldActive->Dimensions.width - 1, 
            oldActive->Dimensions.y + oldActive->Dimensions.height - 1
        );
        ScreenUpdates->Enqueue(WindowUpdate(
            oldActive->Dimensions.x, 
            oldActive->Dimensions.y, 
            oldActive->Dimensions.x + oldActive->Dimensions.width - 1, 
            oldActive->Dimensions.y + oldActive->Dimensions.height - 1
        ));
    }

    if (oldActive != activeWindow && activeWindow != NULL)
    {
        UpdatePointerRect(
            activeWindow->Dimensions.x, 
            activeWindow->Dimensions.y, 
            activeWindow->Dimensions.x + activeWindow->Dimensions.width - 1, 
            activeWindow->Dimensions.y + activeWindow->Dimensions.height - 1
        );
        ScreenUpdates->Enqueue(WindowUpdate(
            activeWindow->Dimensions.x, 
            activeWindow->Dimensions.y, 
            activeWindow->Dimensions.x + activeWindow->Dimensions.width - 1, 
            activeWindow->Dimensions.y + activeWindow->Dimensions.height - 1
        ));
    }
}

#include <libm/keyboard.h>

bool HandleClick(bool L, bool R, bool M)
{
    bool res = false;
    if (L)
    {
        Window* oldActive = activeWindow;
        activeWindow = getWindowAtMousePosition(); 
        
        // Simple Window Click
        if (activeWindow != NULL)
        {
            MakeWinActive(oldActive, activeWindow);
        }

        // Window Start Drag
        if (currentActionWindow == NULL || (activeWindow != NULL && activeWindow != currentActionWindow))
        {
            if (activeWindow != NULL)
            {
                if (activeWindow->ShowTitleBar && activeWindow->Moveable)
                {
                    windowDragMouseStart = MousePosition;
                    windowDragWindowStart = activeWindow->Dimensions;
                    for (int i = 0; i < 4; i++)
                    {
                        windowDragMouseArr[i] = dragArr[i];
                        doingWindowDrag |= windowDragMouseArr[i];
                    }

                    if (!doingWindowDrag &&
                        MousePosition.y < activeWindow->Dimensions.y &&
                        MousePosition.y > activeWindow->Dimensions.y - 22)
                        doingWindowDrag = true;
                        
                    

                    res |= doingWindowDrag;

                    // if (MousePosition.y < activeWindow->Dimensions.y + 22)
                    // {
                    //     if (MousePosition.y > activeWindow->Dimensions.y)
                    //     {
                    //         doingWindowDrag = true;
                    //         res = true;
                    //     }
                    // }
                    // else
                    // {
                    //     doingWindowDrag = false;
                    //     for (int i = 0; i < 4; i++)
                    //     {
                    //         windowDragMouseArr[i] = dragArr[i];
                    //         doingWindowDrag |= windowDragMouseArr[i];
                    //     }
                    //     res |= doingWindowDrag;
                    // }
                }
            }
        }
        // Window Action
        else if (activeWindow == currentActionWindow && activeWindow != NULL) 
        {
            WindowActionEnum action = GetCurrentAction(activeWindow);

            if (action == WindowActionEnum::_NONE)
            {

            }
            else if (action == WindowActionEnum::CLOSE)
            {
                res = true;
                // if shift, fully close the process
                if (envGetKeyState(Key_GeneralShift))
                {
                    closeProcess(activeWindow->PID);
                }
                else
                {
                    GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::WINDOW_DELETE_EVENT, NULL, 0);
                    msgSendConv(packet, activeWindow->PID, activeWindow->CONVO_ID_WM_WINDOW_CLOSED);
                    packet->Free();
                    _Free(packet);
                }

                AddWindowToBeRemoved(activeWindow);
            }
            else if (action == WindowActionEnum::HIDE)
            {
                activeWindow->Hidden = !activeWindow->Hidden;
                activeWindow = NULL;
                res = true;
            }
            else if (action == WindowActionEnum::MIN_MAX)
            {
                serialPrintLn("Warning: Min/Max not implemented yet!");
                res = true;
            }
        }
        // SUS
        else
        {
            serialPrintLn("Warning: Active Window and Current Action Window are not the same!");
        }

        // Taskbar Click
        if (activeWindow == NULL && Taskbar::activeTabWindow != NULL)
        {
            activeWindow = oldActive;
            if (activeWindow != Taskbar::activeTabWindow) // any other window (including null)
            {
                MakeWinActive(oldActive, Taskbar::activeTabWindow);
                if (activeWindow != NULL)
                    activeWindow->Hidden = false;
            }
            else // same window (cant be null)
            {
                activeWindow->Hidden = true;
                activeWindow = NULL;
            }
            res = true;
        }
        else if (Taskbar::MButtonSelected)
        {
            if (startMenuWindow != NULL)
            {
                if (startMenuWindow->Hidden)
                {
                    startMenuWindow->Hidden = false;
                    MakeWinActive(activeWindow, startMenuWindow);
                }
                else
                {
                    startMenuWindow->Hidden = true;
                    MakeWinActive(activeWindow, NULL);
                }
            }
            res = true;
        }
    }
    else if (M)
    {
        if (Taskbar::activeTabWindow != NULL)
        {
            res = true;

            // if shift, fully close the process
            if (envGetKeyState(Key_GeneralShift))
            {
                closeProcess(Taskbar::activeTabWindow->PID);
            }
            else
            {
                GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::WINDOW_DELETE_EVENT, NULL, 0);
                msgSendConv(packet, Taskbar::activeTabWindow->PID, Taskbar::activeTabWindow->CONVO_ID_WM_WINDOW_CLOSED);
                packet->Free();
                _Free(packet);
            }
            AddWindowToBeRemoved(Taskbar::activeTabWindow);
        }
    }

    return res;
}

bool HandleDrag(bool L, bool R, bool M)
{
    bool res = true;
    if (L)
    {
        doingWindowDrag &= activeWindow != NULL;
        if (doingWindowDrag)
        {
            res = true;
            int dx = MousePosition.x - windowDragMouseStart.x;
            int dy = MousePosition.y - windowDragMouseStart.y;

            if (windowDragMouseArr[0])
            {
                activeWindow->Dimensions.x = windowDragWindowStart.x + dx;
                activeWindow->Dimensions.width = windowDragWindowStart.width - dx;

                if (activeWindow->Dimensions.width < 100)
                {
                    activeWindow->Dimensions.x = windowDragWindowStart.x + windowDragWindowStart.width - 100;
                    activeWindow->Dimensions.width = 100;
                }
            }
            else if (windowDragMouseArr[2])
            {
                activeWindow->Dimensions.width = windowDragWindowStart.width + dx;

                if (activeWindow->Dimensions.width < 100)
                {
                    activeWindow->Dimensions.width = 100;
                }
            }

            if (windowDragMouseArr[1])
            {
                activeWindow->Dimensions.y = windowDragWindowStart.y + dy;
                activeWindow->Dimensions.height = windowDragWindowStart.height - dy;

                if (activeWindow->Dimensions.height < 100)
                {
                    activeWindow->Dimensions.y = windowDragWindowStart.y + windowDragWindowStart.height - 100;
                    activeWindow->Dimensions.height = 100;
                }
            }
            else if (windowDragMouseArr[3])
            {
                activeWindow->Dimensions.height = windowDragWindowStart.height + dy;

                if (activeWindow->Dimensions.height < 100)
                {
                    activeWindow->Dimensions.height = 100;
                }
            }

            bool noSideDrag = true;
            for (int i = 0; i < 4; i++)
                noSideDrag &= !windowDragMouseArr[i];

            if (noSideDrag)
            {
                activeWindow->Dimensions.x = windowDragWindowStart.x + dx;
                activeWindow->Dimensions.y = windowDragWindowStart.y + dy;
            }
        }
    }

    return res;
}

