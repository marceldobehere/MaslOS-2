#include "keyPacket.h"

KeyMessagePacket::KeyMessagePacket(KeyMessagePacketType type, int scancode, char keyChar)
{
    this->Type = type;
    this->Scancode = scancode;
    this->KeyChar = keyChar;
}


