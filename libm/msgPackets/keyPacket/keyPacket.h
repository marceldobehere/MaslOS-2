#pragma once
#include "../msgPacket.h"

enum KeyMessagePacketType : uint8_t
{
    KEY_PRESSED,
    KEY_RELEASE
};

class KeyMessagePacket
{
public:
    KeyMessagePacketType Type;
    int Scancode;
    char KeyChar;

    KeyMessagePacket(KeyMessagePacketType type, int scancode, char keyChar);
};