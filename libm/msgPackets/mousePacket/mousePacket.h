#pragma once
#include "../msgPacket.h"

enum MouseMessagePacketType : uint8_t
{
    MOUSE_MOVE,
    MOUSE_CLICK,
};

struct MouseMessagePacket
{
public:
    int MouseX, MouseY;
    MouseMessagePacketType Type;
    bool Left, Right, Middle;
    bool PrevLeft, PrevRight, PrevMiddle;

    MouseMessagePacket(int mouseX, int mouseY);
};