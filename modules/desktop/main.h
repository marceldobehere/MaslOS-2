#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/syscallManager.h>
#include <libm/env/env.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/cstr.h>
#include <libm/list/list_window.h>
#include <libm/memStuff.h>
#include <libm/list/list_basic_msg.h>
#include <libm/images/bitmapImage.h>
#include <libm/list/list_basic_msg.h>

struct WindowIconEntry
{
    Window* window;
    ImageStuff::BitmapImage* icon;

    WindowIconEntry(Window* window, ImageStuff::BitmapImage* icon)
    {
        this->window = window;
        this->icon = icon;
    }

    void Free()
    {
        if (icon != NULL)
        {
            _Free(icon->imageBuffer);
            _Free(icon);
        }
        icon = NULL;
    }
};

extern List<void*>* windowIconEntries;

static const int countOfButtonIcons = 9;

    static struct _windowButtonIconEnum
    {
        static const int 
        CLOSE_N = 0,
        CLOSE_S = 1,
        CLOSE_H = 2,

        MIN_N = 3,
        MIN_S = 4,
        MIN_H = 5,

        HIDE_N = 6,
        HIDE_S = 7,
        HIDE_H = 8;

    } windowButtonIconEnum;

static const char* const windowButtonIconNames[countOfButtonIcons] = {
    "close_n.mbif", "close_s.mbif", "close_h.mbif", 
    "min_n.mbif",   "min_s.mbif",   "min_h.mbif", 
    "hide_n.mbif",  "hide_s.mbif",  "hide_h.mbif"
};


static struct _windowIconEnum
{
    static const int 
    DEBUG = 0,
    DEFAULT = 1,
    TASK_MANAGER = 2,
    TERMINAL = 3,
    WARNING = 4,
    CRASH = 5,
    GUI_PGM = 6,
    TESTO_PGM = 7;

} windowIconEnum;

static const int countOfWindowIcons = 8;

static const char* windowIconNames[countOfWindowIcons] = {
    "debug.mbif",
    "generic.mbif",
    "task_manager.mbif",
    "terminal.mbif",
    "warning.mbif",
    "crash.mbif",
    "gui.mbif",
    "testo_pgm.mbif"
};

extern ImageStuff::BitmapImage* windowButtonIcons[countOfButtonIcons];

extern ImageStuff::BitmapImage* internalWindowIcons[countOfWindowIcons];


extern TempRenderer* actualScreenRenderer;
extern Framebuffer* actualScreenFramebuffer;
extern Framebuffer* mainBuffer;
extern PointerBuffer* pointerBuffer;

extern uint32_t defaultBackgroundColor;
extern Framebuffer* backgroundImage;
extern bool drawBackground;

extern Framebuffer* taskbar;

extern List<Window*>* windows;
extern List<Window*>* windowsToDelete;
extern Window* activeWindow;
extern Window* currentActionWindow;
extern Window* startMenuWindow;

extern MPoint MousePosition;

#include <libm/queue/queue_window_update.h>

extern Queue<WindowUpdate>* ScreenUpdates;

void InitStuff();

void PrintFPS(int fps, int aFps, int frameTime, int breakTime, int totalTime, uint64_t totalPixelCount, int frameCount);

int main(int argc, char** argv);

uint64_t DrawFrame();

void CheckForDeadWindows();

Window* getWindowAtMousePosition(int dis);
Window* getWindowAtMousePosition();

void AddWindowToBeRemoved(Window* window);