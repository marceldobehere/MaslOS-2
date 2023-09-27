#include "windowBufferUpdatePacket.h"
#include <libm/memStuff.h>
#include <libm/stubs.h>

WindowBufferUpdatePacket::WindowBufferUpdatePacket(int x, int y, int width, int height, uint64_t windowId, uint32_t* buffer)
{
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
    WindowBufferUpdatePacket* packet = (WindowBufferUpdatePacket*)genericMessagePacket->Data;
    this->X = packet->X;
    this->Y = packet->Y;
    this->Width = packet->Width;
    this->Height = packet->Height;
    this->WindowId = packet->WindowId;
    uint32_t* buffer = (uint32_t*)(genericMessagePacket->Data + sizeof(WindowBufferUpdatePacket));
    this->Buffer = (uint32_t*)_Malloc(sizeof(uint32_t) * Width * Height);
    _memcpy(buffer, this->Buffer, sizeof(uint32_t) * Width * Height);
}

GenericMessagePacket* WindowBufferUpdatePacket::ToGenericMessagePacket()
{
    int siz = sizeof(WindowBufferUpdatePacket);
    int fullSize = siz + sizeof(uint32_t) * Width * Height;
    uint8_t* tBuff = (uint8_t*)_Malloc(siz + fullSize);
    _memcpy(this, tBuff, siz);
    _memcpy(Buffer, tBuff + siz, fullSize - siz);
    GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::WINDOW_BUFFER_EVENT, tBuff, fullSize);
    _Free(tBuff);
    return packet;
}

void WindowBufferUpdatePacket::Free()
{
    if (Buffer != NULL)
    {
        _Free(Buffer);
        Buffer = NULL;
    }
}
