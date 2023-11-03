#include "window.h"
#include <libm/stubs.h>
#include <libm/memStuff.h>
#include <libm/math.h>
#include <libm/cstrTools.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>

Window::Window()
{
    Title = NULL;
    OldTitle = NULL;

    Dimensions = WindowDimension();
    OldDimensions = Dimensions;

    ShowTitleBar = true;
    ShowBorder = true;
    Hidden = false;
    Moveable = true;
    Resizeable = true;
    Closeable = true;

    DefaultBorderColor = Colors.dgray;
    SelectedBorderColor = Colors.bgreen;
    DefaultTitleColor = Colors.gray;
    SelectedTitleColor = Colors.white;
    DefaultTitleBackgroundColor = Colors.dgray;


    OldShowTitleBar = ShowTitleBar;
    OldShowBorder = ShowBorder;
    OldHidden = Hidden;

    CurrentBorderColor = DefaultBorderColor;
    CurrentTitleColor = DefaultTitleColor;
    CurrentTitleBackgroundColor = DefaultTitleBackgroundColor;

    OldBorderColor = CurrentBorderColor;
    OldTitleColor = CurrentTitleColor;
    OldTitleBackgroundColor = CurrentTitleBackgroundColor;

    Updates = NULL;
    Buffer = NULL;

    ID = 0;
    PID = 0;
}

Window::Window(int x, int y, int width, int height, const char* title, uint64_t id, uint64_t pid)
{
    Title = StrCopy(title);
    OldTitle = StrCopy(Title);

    if (width < 0)
        width = 0;
    if (height < 0)
        height = 0;
    
    Dimensions = WindowDimension(x, y, width, height);
    OldDimensions = Dimensions;

    ShowTitleBar = true;
    ShowBorder = true;
    Hidden = false;
    Moveable = true;
    Resizeable = true;
    Closeable = true;

    DefaultBorderColor = Colors.dgray;
    SelectedBorderColor = Colors.bgreen;
    DefaultTitleColor = Colors.gray;
    SelectedTitleColor = Colors.white;
    DefaultTitleBackgroundColor = Colors.dgray;

    OldShowTitleBar = ShowTitleBar;
    OldShowBorder = ShowBorder;
    OldHidden = Hidden;

    CurrentBorderColor = DefaultBorderColor;
    CurrentTitleColor = DefaultTitleColor;
    CurrentTitleBackgroundColor = DefaultTitleBackgroundColor;

    OldBorderColor = CurrentBorderColor;
    OldTitleColor = CurrentTitleColor;
    OldTitleBackgroundColor = CurrentTitleBackgroundColor;

    
    Updates = new List<WindowUpdate>();
    Buffer = NULL;

    ID = id;
    PID = pid;

    ResizeFramebuffer(width, height);
}


void Window::ResizeFramebuffer(int width, int height)
{
    Framebuffer* oldBuffer = Buffer;
    
    Buffer = (Framebuffer*)_Malloc(sizeof(Framebuffer), "Framebuffer");
    Buffer->Width = width;
    Buffer->Height = height;
    Buffer->PixelsPerScanLine = width;
    Buffer->BufferSize = width * height * 4;
    Buffer->BaseAddress = _Malloc(Buffer->BufferSize, "Framebuffer Data");

    // possibly optimize by only doing a memset if buffer is null and if it isnt then only clear the empty data
    _memset(Buffer->BaseAddress, 0, Buffer->BufferSize);

    if (oldBuffer != NULL)
    {
        int minX = min(oldBuffer->Width, Buffer->Width);
        int minY = min(oldBuffer->Height, Buffer->Height);

        uint32_t* oldData = (uint32_t*)oldBuffer->BaseAddress;
        uint32_t* newData = (uint32_t*)Buffer->BaseAddress;

        for (int y = 0; y < minY; y++)
            for (int x = 0; x < minX; x++)
                newData[x + y * Buffer->PixelsPerScanLine] = oldData[x + y * oldBuffer->PixelsPerScanLine];

        _Free(oldBuffer->BaseAddress);
        _Free(oldBuffer);
    }

    Updates->Add(WindowUpdate(0, 0, width, height));
}

void Window::_CheckDimensionChange()
{
    if (Dimensions.x != OldDimensions.x ||
        Dimensions.y != OldDimensions.y)
    {
        int x1 = OldDimensions.x;
        int y1 = OldDimensions.y;

        int x2 = OldDimensions.x + OldDimensions.width;
        int y2 = OldDimensions.y + OldDimensions.height;

        int x3 = Dimensions.x;
        int y3 = Dimensions.y;

        int x4 = Dimensions.x + Dimensions.width;
        int y4 = Dimensions.y + Dimensions.height;


        // Apparently virtual box crashes here because of a page fault
        // with some code commented out its either a page fault due to an instruction fetch
        // without code commented out its apparently due to error code 6 -> Some reserved bit was set to 1 (in userspace)
        int minX = min(x1, x3);
        int minY = min(y1, y3);

        int maxX = max(x2, x4);
        int maxY = max(y2, y4);

        minX -= 1;
        minY -= 23;
        maxX += 1;
        maxY += 1;

        if (minX < 0)
            minX = 0;
        if (minY < 0)
            minY = 0;

        int width = maxX - minX + 1;
        int height = maxY - minY + 1;

        if (Dimensions.width != OldDimensions.width ||
            Dimensions.height != OldDimensions.height)
        {
            ResizeFramebuffer(Dimensions.width, Dimensions.height);
            OldDimensions.width = Dimensions.width;
            OldDimensions.height = Dimensions.height;
        }

        Updates->Clear();
        Updates->Add(WindowUpdate(minX - Dimensions.x, minY - Dimensions.y, width, height, true));

        OldDimensions.x = Dimensions.x;
        OldDimensions.y = Dimensions.y;
    }


    if (Dimensions.width != OldDimensions.width ||
        Dimensions.height != OldDimensions.height)
    {
        ResizeFramebuffer(Dimensions.width, Dimensions.height);
        OldDimensions.width = Dimensions.width;
        OldDimensions.height = Dimensions.height;
    }
}

void Window::_CheckTitleChange()
{
    if (!StrEquals(Title, OldTitle))
    {
        _Free(OldTitle);
        OldTitle = StrCopy(Title);

        Updates->Add(WindowUpdate(0, -23, Dimensions.width, 1, true));
    }
}

void Window::_CheckBorderChange()
{
    if (CurrentBorderColor != OldBorderColor)
    {
        // top
        Updates->Add(WindowUpdate(0, -23, Dimensions.width, 0, true));

        // left
        Updates->Add(WindowUpdate(-1, -23, -1, Dimensions.height, true));

        // bottom
        Updates->Add(WindowUpdate(0, Dimensions.height, Dimensions.width, Dimensions.height, true));

        // right
        Updates->Add(WindowUpdate(Dimensions.width, -23, Dimensions.width, Dimensions.height, true));

        OldBorderColor = CurrentBorderColor;
    }

    if (CurrentTitleBackgroundColor != OldTitleBackgroundColor || 
        CurrentTitleColor != OldTitleColor)
    {
        Updates->Add(WindowUpdate(-1, -23, Dimensions.width, 1, true));

        OldTitleBackgroundColor = CurrentTitleBackgroundColor;
        OldTitleColor = CurrentTitleColor;
    }
}

void Window::_CheckVisChange()
{
    if (ShowTitleBar != OldShowTitleBar)
    {
        Updates->Add(WindowUpdate(-1, -23, Dimensions.width, 0, true));
        OldShowTitleBar = ShowTitleBar;
    }
    if (ShowBorder != OldShowBorder)
    {
        // top
        Updates->Add(WindowUpdate(0, -23, Dimensions.width, 0, true));

        // left
        Updates->Add(WindowUpdate(-1, -23, -1, Dimensions.height, true));

        // bottom
        Updates->Add(WindowUpdate(0, Dimensions.height, Dimensions.width, Dimensions.height, true));

        // right
        Updates->Add(WindowUpdate(Dimensions.width, -23, Dimensions.width, Dimensions.height, true));

        OldShowBorder = ShowBorder;
    }
    if (Hidden != OldHidden)
    {
        Updates->Clear();
        Updates->Add(WindowUpdate(-1, -23, Dimensions.width + 1, Dimensions.height + 1, true));

        OldHidden = Hidden;
    }
}

void Window::UpdateCheck()
{
    _CheckBorderChange();

    _CheckTitleChange();

    _CheckDimensionChange();

    _CheckVisChange();
}

void Window::UpdateUsingPartialWindow(Window* window, bool updateIdAndPid)
{
    // TODO apply the changes

    // Title
    if (window->Title != NULL)
    {
        _Free((void*)Title);
        Title = StrCopy(window->Title);
    }

    // Dimensions
    Dimensions = window->Dimensions;

    // ShowTitleBar
    ShowTitleBar = window->ShowTitleBar;
    // ShowBorder
    ShowBorder = window->ShowBorder;
    // Hidden
    Hidden = window->Hidden;
    // Moveable
    Moveable = window->Moveable;
    // Resizeable
    Resizeable = window->Resizeable;
    // Closeable
    Closeable = window->Closeable;

    // DefaultBorderColor
    DefaultBorderColor = window->DefaultBorderColor;
    // SelectedBorderColor
    SelectedBorderColor = window->SelectedBorderColor;
    // DefaultTitleColor
    DefaultTitleColor = window->DefaultTitleColor;
    // SelectedTitleColor
    SelectedTitleColor = window->SelectedTitleColor;
    // DefaultTitleBackgroundColor
    DefaultTitleBackgroundColor = window->DefaultTitleBackgroundColor;

    if (updateIdAndPid)
    {
        // ID
        ID = window->ID;
        // PID
        PID = window->PID;
    }

    UpdateCheck();
}

// void Window::DrawToFramebuffer(Framebuffer* framebuffer, Framebuffer* backbuffer, WindowUpdate update, int x, int y)
// {

// }

void Window::Free()
{
    if (Title != NULL)
    {
        _Free((void*)Title);
        Title = NULL;
    }
    if (OldTitle != NULL)
    {
        _Free((void*)OldTitle);
        OldTitle = NULL;
    }
    if (Buffer != NULL)
    {
        _Free(Buffer->BaseAddress);
        _Free(Buffer);
        Buffer = NULL;
    }
    if (Updates != NULL)
    {
        Updates->Free();
        _Free(Updates);
        Updates = NULL;
    }
}