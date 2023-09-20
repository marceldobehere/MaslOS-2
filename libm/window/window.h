#pragma once
#include <stdint.h>
#include <stddef.h>

class Window;

struct WindowUpdate
{
    int x1;
    int y1;
    int x2;
    int y2;

    WindowUpdate()
    {
        x1 = 0; 
        y1 = 0; 
        x2 = 0; 
        y2 = 0;
    }

    WindowUpdate(int x1, int y1, int x2, int y2)
    {
        this->x1 = x1; 
        this->y1 = y1; 
        this->x2 = x2; 
        this->y2 = y2;
    }

    bool operator ==(const WindowUpdate& other)
    {
        return x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2;
    }

    bool operator !=(const WindowUpdate& other)
    {
        return x1 != other.x1 || y1 != other.y1 || x2 != other.x2 || y2 != other.y2;
    }
};

#include <libm/list/list_window_update.h>

struct WindowDimension
{
    int x;
    int y;
    int width;
    int height;

    WindowDimension()
    {
        x = 0; 
        y = 0; 
        width = 0; 
        height = 0;
    }

    WindowDimension(int x, int y, int width, int height)
    {
        this->x = x; 
        this->y = y; 
        this->width = width; 
        this->height = height;
    }

    bool operator ==(const WindowDimension& other)
    {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }

    bool operator !=(const WindowDimension& other)
    {
        return x != other.x || y != other.y || width != other.width || height != other.height;
    }
};

#include <libm/rendering/framebuffer.h>

class Window
{
    public:
    
    const char* Title;
    const char* OldTitle;
    WindowDimension Dimensions = WindowDimension();
    WindowDimension OldDimensions = WindowDimension();
    
    List<WindowUpdate>* Updates;

    Framebuffer* Buffer;

    Window(int x, int y, int width, int height, const char* title);

    void ResizeFramebuffer(int width, int height);

    void _CheckDimensionChange();

    void _CheckTitleChange();

    void UpdateCheck();

    void Free();
};