#pragma once
#include <stdint.h>
#include <stddef.h>

struct MouseState
{
    int MouseX, MouseY;
    bool Left, Right, Middle;

    MouseState(int x, int y, bool left, bool right, bool middle)
    {
        MouseX = x;
        MouseY = y;
        Left = left;
        Right = right;
        Middle = middle;
    }
};