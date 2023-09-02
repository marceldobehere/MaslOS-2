#pragma once
#include <stdint.h>
#include <stddef.h>

namespace ScancodeTranslation
{
    extern const char defaultScancodeTranslationTable[];
    extern const char defaultShiftScancodeTranslationTable[];

    const int LeftShift = 0x2A;
    const int RightShift = 0x36;

    const int LeftAlt = 0x38;
    const int LeftControl = 0x1D;

    const int Enter = 0x1C;
    const int Escape = 0x01;
    const int Tab = 0x0F;
    const int Backspace = 0x0E;

    const int ArrowUp = 0x48;
    const int ArrowDown = 0x50;
    const int ArrowLeft = 0x4B;
    const int ArrowRight = 0x4D;
    

    char GetSpecialKey(int scancode);
    inline bool IsSpecialKey(int scancode)
    {
        return GetSpecialKey(scancode) != 0;
    }
    
    char TranslateScancode(int scancode, bool shift);
    char TranslateScancode(int scancode, bool shift, const char table[], const char tableShift[]);
}