#pragma once
#include "../msgPacket.h"

class WindowBufferUpdatePacket
{
public:
    int X;
    int Y;
    int Width;
    int Height;
    uint64_t WindowId;
    uint32_t* Buffer;

    WindowBufferUpdatePacket(int x, int y, int width, int height, uint64_t windowId, uint32_t* buffer);
    WindowBufferUpdatePacket(GenericMessagePacket* genericMessagePacket);

    GenericMessagePacket* ToGenericMessagePacket();

    void Free();
};