#include "mousePacket.h"

MouseMessagePacket::MouseMessagePacket(int mouseX, int mouseY)
{
    Type = MOUSE_MOVE;
    MouseX = mouseX;
    MouseY = mouseY;

    Left = false;
    Right = false;
    Middle = false;
}

MouseMessagePacket::MouseMessagePacket(bool hold, bool left, bool right, bool middle)
{
    Type = hold ? MOUSE_HOLD : MOUSE_RELEASE;
    MouseX = 0;
    MouseY = 0;

    Left = left;
    Right = right;
    Middle = middle;
}