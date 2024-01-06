#include "taskbar.h"

#define AddToStack()
#define RemoveFromStack()

#include <libm/stubs.h>
#include <libm/userEnvStuff.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/cstrTools.h>
#include <libm/rendering/Cols.h>

#include "main.h"
#include "wmStuff.h"



namespace Taskbar
{
    Framebuffer* taskbarBuffer;
    TempRenderer* renderer;
    ImageStuff::BitmapImage* MButton;
    ImageStuff::BitmapImage* MButtonS;
    uint32_t backgroundColor;
    uint32_t defaultTabBackgroundColor;
    uint32_t selectedTabBackgroundColor;
    uint32_t defaultTabTextColor;
    uint32_t selectedTabTextColor;
    SyncedList<Window*>* taskWindowList;
    Window* activeTabWindow;
    bool MButtonSelected;

    int Scounter = 0;

    void InitTaskbar(ImageStuff::BitmapImage* mButton, ImageStuff::BitmapImage* mButtonS)
    {
        AddToStack();
        MButton = mButton;
        MButtonS = mButtonS;
        taskbarBuffer = taskbar;//osData.windowPointerThing->taskbar;
        renderer = (TempRenderer*)_Malloc(sizeof(TempRenderer), "New Basic Renderer for Taskbar");
        *renderer = TempRenderer(taskbarBuffer, defaultRenderFont);
        
        taskWindowList = (SyncedList<Window*>*)_Malloc(sizeof(SyncedList<Window*>), "new Synced List for Taskbar");
        *taskWindowList = SyncedList<Window*>(windows);

        backgroundColor =            0xff001530;
        defaultTabBackgroundColor =  0xff002299;
        selectedTabBackgroundColor = 0xff0030BF;
        defaultTabTextColor =        0xffABABAB;
        selectedTabTextColor =       0xffFFFFFF;
        MButtonSelected = false;

        Scounter = 1000;

        RemoveFromStack();
    }

    

    bool RenderTaskbar()
    {
        int checkVal = 19;
        if (MousePosition.y > mainBuffer->Height - taskbarBuffer->Height - 20)
            checkVal = 5;
        else
            activeTabWindow = NULL;

        if (Scounter++ < checkVal)
            return false;
        Scounter = 0;


        AddToStack();

        taskbarBuffer = taskbar;//osData.windowPointerThing->taskbar;
        renderer->framebuffer = taskbarBuffer;


        AddToStack();
        {
            uint32_t* endAddr = (uint32_t*)((uint64_t)taskbarBuffer->BaseAddress + taskbarBuffer->BufferSize);
            uint32_t col = backgroundColor;
            for (uint32_t* addr = (uint32_t*)taskbarBuffer->BaseAddress; addr < endAddr; addr++)
                *addr = col;
        }
        RemoveFromStack();

        

        AddToStack();
        int64_t ypos = mainBuffer->Height - taskbarBuffer->Height;
        RemoveFromStack();

        

        AddToStack();
        MButtonSelected = (MousePosition.x >= 0 && MousePosition.x < 40 && MousePosition.y >= ypos && MousePosition.y < ypos + 40);
        if (MButtonSelected)
            GlobalDrawImage(MButtonS, 0, 0, 1, 1, taskbarBuffer);
        else
            GlobalDrawImage(MButton, 0, 0, 1, 1, taskbarBuffer);
        RemoveFromStack();
        
        
        //GlobDrawImage(currentMouseImage, 10, 10, 1, 1, taskbarBuffer);
        AddToStack();
        if (Scounter % 5 == 0)
            taskWindowList->Sync();
        RemoveFromStack();

        AddToStack();
        int64_t wCount = taskWindowList->GetCount();
        int64_t size = 200;
        int64_t width = taskbarBuffer->Width;
        int64_t height = taskbarBuffer->Height;
        RemoveFromStack();

        AddToStack();
        if (wCount != 0)
        {
            size = (width - 400) / wCount;
            if (size > 200)
                size = 200;
            if (size < 60)
                size = 60;
        }
        RemoveFromStack();

        AddToStack();
        {
            AddToStack();
            int64_t x = 46;
            int iconSize = 24;
            int ydiff = ((height)-iconSize)/2;
            int textStart = iconSize + 12;
            Window* tempWindow = NULL;
            RemoveFromStack();

            for (int i = 0; i < wCount; i++)
            {
                if (x + size > width)
                    break;

                AddToStack();
                Window* window = taskWindowList->ElementAt(i);
                RemoveFromStack();

                // if (window == osData.debugTerminalWindow && !osData.showDebugterminal)
                //     continue;
                if (window == startMenuWindow)
                    continue;

                bool hover = window == activeWindow;

                if (MousePosition.x >= x && MousePosition.x < (x + size) && MousePosition.y >= ypos && MousePosition.y < mainBuffer->Height)
                {
                    hover = true;
                    tempWindow = window;
                }

                AddToStack();
                if (hover)
                    renderer->Clear(x, 2, x + size, height - 3, selectedTabBackgroundColor); // Clear whole Rect
                else
                    renderer->Clear(x, 2, x + size, height - 3, defaultTabBackgroundColor); // Clear whole Rect
                RemoveFromStack();

                // Get the correct icon
                ImageStuff::BitmapImage* icon = internalWindowIcons[windowIconEnum.TERMINAL];
                for (int i = 0; i < windowIconEntries->GetCount(); i++)
                {
                    AddToStack();
                    WindowIconEntry* entry = (WindowIconEntry*)windowIconEntries->ElementAt(i);
                    RemoveFromStack();

                    if (entry->window == window && entry->icon != NULL)
                    {
                        icon = entry->icon;
                        break;
                    }
                }

                AddToStack();
                bool drawIcon = (icon != NULL);
                if (drawIcon)
                    drawIcon = (icon->height == iconSize && icon->width == iconSize);
                RemoveFromStack();

                AddToStack();
                if (drawIcon)
                    renderer->DrawImage(icon, x + 6, ydiff, 1, 1);
                else
                    renderer->Clear(x + 6, ydiff, x + 5 + iconSize, height - (ydiff+1), Colors.black); // Clear Icon Rect
                RemoveFromStack();

                AddToStack();
                {
                    AddToStack();
                    char* stitle = StrSubstr(window->Title, 0, ((size-8) - textStart) / 8);

                    int strLen1 = ((size-8) - textStart) / 8;
                    int strLen2 = StrLen(window->Title);
                    RemoveFromStack();

                    AddToStack();
                    if (strLen1 < strLen2)
                    {
                        int amount = min(3, strLen1);
                        int index = strLen1 - 1;
                        for (int i = 0; i < amount; i++)
                        {
                            stitle[index] = '.';
                            index--;
                        }
                    }
                    RemoveFromStack();

                    AddToStack();
                    if (hover)
                        renderer->color = selectedTabTextColor;
                    else
                        renderer->color = defaultTabTextColor;
                    RemoveFromStack();

                    renderer->putStr(stitle, x + textStart, (height/2)-6);
                    _Free((void*)stitle);
                }
                RemoveFromStack();

                activeTabWindow = tempWindow;



                x += size + 6;
            }
        }
        RemoveFromStack();


        AddToStack();
        char tempTime[9];
        const char* tempTimeTemplate = "00:00:00";
        for (int i = 0; i < 9; i++)
            tempTime[i] = tempTimeTemplate[i];
        char tempDate[11];
        const char* tempDateTemplate = "00.00.0000";
        for (int i = 0; i < 11; i++)
            tempDate[i] = tempDateTemplate[i];
        
        const char* tX = "";
        int tL = 0;

        //rtc->UpdateTimeIfNeeded();
        
        
        RTC_Info* rtc = envGetTimeRTC();

        // Time
        if (rtc != NULL)
        {
            tX = to_string(rtc->Hour);
            tL = StrLen(tX);
            for (int i = 0; i < tL; i++)
                tempTime[i + 0 + (2 - tL)] = tX[i];

            tX = to_string(rtc->Minute);
            tL = StrLen(tX);
            for (int i = 0; i < tL; i++)
                tempTime[i + 3 + (2 - tL)] = tX[i];

            tX = to_string(rtc->Second);
            tL = StrLen(tX);
            for (int i = 0; i < tL; i++)
                tempTime[i + 6 + (2 - tL)] = tX[i];    
        }

        // Date
        if (rtc != NULL)
        {
            tX = to_string(rtc->Day);
            tL = StrLen(tX);
            for (int i = 0; i < tL; i++)
                tempDate[i + 0 + (2 - tL)] = tX[i];

            tX = to_string(rtc->Month);
            tL = StrLen(tX);
            for (int i = 0; i < tL; i++)
                tempDate[i + 3 + (2 - tL)] = tX[i];

            tX = to_string(rtc->Year);
            tL = StrLen(tX);
            for (int i = 0; i < tL; i++)
                tempDate[i + 6 + (4 - tL)] = tX[i];
        }

        if (rtc != NULL)
            _Free(rtc);


        //renderer->Clear(width - 8*13, 0, width - 1, height - 1, Colors.orange);
        
        renderer->color = Colors.white;
        renderer->CursorPosition.x = width - 8*12 + 8*2 - 4;
        renderer->CursorPosition.y = ((height - 32) / 2);
        renderer->printStr(tempTime);

        renderer->color = Colors.white;
        renderer->CursorPosition.x = width - 8*12 + 8*1 - 4;
        renderer->CursorPosition.y = ((height - 32) / 2) + 16;
        renderer->printStr(tempDate);

        RemoveFromStack();


        UpdatePointerRect(0, ypos, taskbarBuffer->Width - 1, ypos + taskbarBuffer->Height - 1);
        RemoveFromStack();

        return true;
    }
}