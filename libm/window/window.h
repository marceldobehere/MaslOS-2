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
    bool outsideWindow;

    WindowUpdate()
    {
        x1 = 0; 
        y1 = 0; 
        x2 = 0; 
        y2 = 0;
        outsideWindow = false;
    }

    WindowUpdate(int x1, int y1, int x2, int y2)
    {
        this->x1 = x1; 
        this->y1 = y1; 
        this->x2 = x2; 
        this->y2 = y2;
        this->outsideWindow = false;
    }

    WindowUpdate(int x1, int y1, int x2, int y2, bool outsideWindow)
    {
        this->x1 = x1; 
        this->y1 = y1; 
        this->x2 = x2; 
        this->y2 = y2;
        this->outsideWindow = outsideWindow;
    }

    bool operator ==(const WindowUpdate& other)
    {
        return x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2 && outsideWindow == other.outsideWindow;
    }

    bool operator !=(const WindowUpdate& other)
    {
        return x1 != other.x1 || y1 != other.y1 || x2 != other.x2 || y2 != other.y2 || outsideWindow != other.outsideWindow;
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

    bool ShowTitleBar;
    bool ShowBorder;
    bool Hidden;
    bool Moveable;
    bool Resizeable;
    bool Closeable;

    bool IsActive;
    bool IsFrozen;

    bool OldShowTitleBar;
    bool OldShowBorder;
    bool OldHidden;
    bool OldIsActive;
    bool OldIsFrozen;

    uint32_t DefaultBorderColor;
    uint32_t SelectedBorderColor;
    uint32_t DefaultTitleColor;
    uint32_t SelectedTitleColor;
    uint32_t DefaultTitleBackgroundColor;
    uint32_t DefaultBackgroundColor;

    uint32_t CurrentBorderColor;
    uint32_t CurrentTitleColor;
    uint32_t CurrentTitleBackgroundColor;

    uint32_t OldBorderColor;
    uint32_t OldTitleColor;
    uint32_t OldTitleBackgroundColor;

    uint64_t ID;
    uint64_t PID;

    uint64_t CONVO_ID_WM_MOUSE_STUFF;
    uint64_t CONVO_ID_WM_KB_STUFF;
    uint64_t CONVO_ID_WM_WINDOW_UPDATE;
    uint64_t CONVO_ID_WM_WINDOW_CLOSED;
    
    List<WindowUpdate>* Updates;

    Framebuffer* Buffer;

    Window();
    Window(uint64_t id);
    Window(int x, int y, int width, int height, const char* title, uint64_t id, uint64_t pid);

    void UpdateUsingPartialWindow(Window* window, bool updateId, bool updatePid, bool updateActive);

    void ResizeFramebuffer(int width, int height);

    void _CheckDimensionChange();

    void _CheckTitleChange();

    void _CheckBorderChange();

    void _CheckVisChange();

    void UpdateCheck();
    
    void Free();
};