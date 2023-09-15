#pragma once
#include <stdint.h>
#include <stddef.h>
//#include "../stubs.h"
#include "../heap/heap.h"

enum MessagePacketType : uint8_t
{
    NONE,
    GENERIC_DATA,
    GENERIC_MESSAGE,
    KEY_EVENT,
    MOUSE_EVENT
};

class GenericMessagePacket
{
public:
    MessagePacketType Type;
    uint8_t* Data;
    uint64_t Size;

    GenericMessagePacket(MessagePacketType type, uint8_t* data, uint64_t size);
    GenericMessagePacket* Copy();
    void Free();

    GenericMessagePacket(MessagePacketType type, uint8_t* data, uint64_t size, Heap::HeapManager* manager);
    GenericMessagePacket* Copy(Heap::HeapManager* manager);
    void Free(Heap::HeapManager* manager);
};