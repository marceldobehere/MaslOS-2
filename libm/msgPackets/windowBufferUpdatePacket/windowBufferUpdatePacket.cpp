#include "windowBufferUpdatePacket.h"
#include <libm/memStuff.h>
#include <libm/stubs.h>

WindowBufferUpdatePacket::WindowBufferUpdatePacket(int x, int y, int width, int height, uint64_t windowId, uint32_t* buffer, bool isRef)
{
    internalPacket = NULL;
    this->isRef = isRef;
    this->X = x;
    this->Y = y;
    this->Width = width;
    this->Height = height;
    this->WindowId = windowId;
    if (isRef)
        this->Buffer = buffer;
    else
    {
        this->Buffer = (uint32_t*)_Malloc(sizeof(uint32_t) * width * height);
        _memcpy(buffer, this->Buffer, sizeof(uint32_t) * width * height);
    }
}

WindowBufferUpdatePacket::WindowBufferUpdatePacket(int x, int y, int width, int height, uint64_t windowId, uint32_t* buffer)
{
    internalPacket = NULL;
    isRef = false;
    this->X = x;
    this->Y = y;
    this->Width = width;
    this->Height = height;
    this->WindowId = windowId;
    this->Buffer = (uint32_t*)_Malloc(sizeof(uint32_t) * width * height);
    _memcpy(buffer, this->Buffer, sizeof(uint32_t) * width * height);
}
WindowBufferUpdatePacket::WindowBufferUpdatePacket(GenericMessagePacket* genericMessagePacket)
{
    internalPacket = genericMessagePacket;
    isRef = true;
    WindowBufferUpdatePacket* packet = (WindowBufferUpdatePacket*)genericMessagePacket->Data;
    this->X = packet->X;
    this->Y = packet->Y;
    this->Width = packet->Width;
    this->Height = packet->Height;
    this->WindowId = packet->WindowId;
    this->Buffer = (uint32_t*)(genericMessagePacket->Data + sizeof(WindowBufferUpdatePacket));
}

GenericMessagePacket* WindowBufferUpdatePacket::ToGenericMessagePacket()
{
    int siz = sizeof(WindowBufferUpdatePacket);
    int fullSize = siz + sizeof(uint32_t) * Width * Height;
    GenericMessagePacket* packet = new GenericMessagePacket(fullSize, MessagePacketType::WINDOW_BUFFER_EVENT);
    _memcpy(this, packet->Data, siz);
    _memcpy(Buffer, packet->Data + siz, fullSize - siz);
    return packet;
}

void WindowBufferUpdatePacket::Free()
{
    if (internalPacket != NULL)
    {
        internalPacket->Free();
        _Free(internalPacket);
        internalPacket = NULL;
        Buffer = NULL;
    }
    else if (Buffer != NULL)
    {
        if (!isRef)
            _Free(Buffer);
        Buffer = NULL;
    }
}
