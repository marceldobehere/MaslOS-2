#include "mousePacket.h"

MouseMessagePacket::MouseMessagePacket(int mouseX, int mouseY)
{
    Type = MOUSE_MOVE;
    MouseX = mouseX;
    MouseY = mouseY;

    Left = false;
    Right = false;
    Middle = false;

    PrevLeft = false;
    PrevRight = false;
    PrevMiddle = false;
}
