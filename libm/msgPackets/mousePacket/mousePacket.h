#pragma once
#include "../msgPacket.h"

enum MouseMessagePacketType : uint8_t
{
    MOUSE_MOVE,
    MOUSE_HOLD,
    MOUSE_RELEASE,
};

class MouseMessagePacket
{
public:
    MouseMessagePacketType Type;
    int MouseX, MouseY;
    bool Left, Right, Middle;

    MouseMessagePacket(int mouseX, int mouseY);
    MouseMessagePacket(bool hold, bool left, bool right, bool middle);
};