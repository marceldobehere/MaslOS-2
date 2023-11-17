#pragma once
#include "window.h"

enum class WindowAttributeType : int32_t
{   
    POSITION_X = 10,
    POSITION_Y = 11,

    SIZE_X = 20,
    SIZE_Y = 21,

    ID = 30, 


    DEFAULT_BORDER_COLOR = 40,
    SELECTED_BORDER_COLOR = 41,

    DEFAULT_TITLE_COLOR = 42,
    SELECTED_TITLE_COLOR = 43,
    DEFAULT_TITLE_BACKGROUND_COLOR = 44,
    

    IS_TITLEBAR_SHOWN = 50,
    IS_BORDER_SHOWN = 51,
    IS_WINDOW_MOVEABLE = 52,
    IS_WINDOW_SHOWN = 53,
    IS_WINDOW_RESIZEABLE = 54,
    IS_WINDOW_SELECTED = 55,

    TITLE_TEXT = 60
};

// Window* FindWindowWithId(int64_t id);

// bool DeleteWindowWithId(int64_t id);
// bool CreateWindowWithId(int64_t id);


bool SetWindowAttribute(Window* window, WindowAttributeType type, uint64_t val);
uint64_t GetWindowAttribute(Window* window, WindowAttributeType type);
int GetWindowAttributeSize(WindowAttributeType type);