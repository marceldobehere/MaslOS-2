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

        _Free(Buffer->BaseAddress);
        _Free(Buffer);
    }

    Updates->Add(WindowUpdate(0, 0, width, height));
}

void Window::_CheckDimensionChange()
{
    
}

void Window::_CheckTitleChange()
{
    if (!StrEquals(Title, OldTitle))
    {
        _Free(OldTitle);
        OldTitle = StrCopy(Title);

        Updates->Add(WindowUpdate(0, -23, Dimensions.width, 1));
    }
}

void Window::UpdateCheck()
{
    _CheckTitleChange();
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