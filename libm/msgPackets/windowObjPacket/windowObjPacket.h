#pragma once
#include "../msgPacket.h"
#include <libm/window/window.h>

class WindowObjectPacket
{
public:
    Window* PartialWindow;
    bool Set; // true -> set, false -> get

    WindowObjectPacket(Window* window, bool set);
    WindowObjectPacket(GenericMessagePacket* genericMessagePacket);
    GenericMessagePacket* ToGenericMessagePacket();

    void Free();
};

