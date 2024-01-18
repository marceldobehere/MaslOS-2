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
    bool isRef;
    GenericMessagePacket* internalPacket = NULL;

    WindowBufferUpdatePacket(int x, int y, int width, int height, uint64_t windowId, uint32_t* buffer);
    WindowBufferUpdatePacket(int x, int y, int width, int height, uint64_t windowId, uint32_t* buffer, bool isRef);
    WindowBufferUpdatePacket(GenericMessagePacket* genericMessagePacket);

    GenericMessagePacket* ToGenericMessagePacket();

    void Free();
};