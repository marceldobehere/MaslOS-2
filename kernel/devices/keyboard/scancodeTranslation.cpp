#include "scancodeTranslation.h"

namespace ScancodeTranslation
{
    const char defaultScancodeTranslationTable[] = 
    {
        0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0', 
        '\\', '`', '\b',  0 ,
        'q', 'w', 'e', 'r',
        't', 'z', 'u', 'i',
        'o', 'p', '[', '+',
        '\n',  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', '<',
        '{','{',  '~' , '#',
        'y', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '-',  0 , '*',
         0 , ' '
    };

    const char defaultShiftScancodeTranslationTable[] = 
    {
         0 ,  0 , '!', '"',
         35, '$', '%', '&',
        '/', '(', ')', '=', 
        '?', '~',  0 ,  0 ,
        'Q', 'W', 'E', 'R',
        'T', 'Z', 'U', 'I',
        'O', 'P', ']', '*',
         0 ,  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', '>',
        '}','}',  0 , '\'',
        'Y', 'X', 'C', 'V',
        'B', 'N', 'M', ';',
        ':', '_',  0 , '~',
         0 , ' '
    };

    char TranslateScancode(int scancode, bool shift)
    {
        return TranslateScancode(scancode, shift, defaultScancodeTranslationTable, defaultShiftScancodeTranslationTable);
    }

    char GetSpecialKey(int scancode)
    {
        if (scancode == ArrowUp) return 0x48;
        if (scancode == ArrowDown) return 0x50;
        if (scancode == ArrowLeft) return 0x4B;
        if (scancode == ArrowRight) return 0x4D;

        if (scancode == LeftControl) return 0x1D;
        if (scancode == LeftShift) return 0x2A;
        if (scancode == RightShift) return  0x36;
        if (scancode == LeftAlt) return 0x38;

        if (scancode == Backspace) return 0x0E;
        if (scancode == Enter) return '\n';
        if (scancode == Escape) return  0x01;
        if (scancode == Tab) return 0x0F;

        return 0;
    }

    char TranslateScancode(int scancode, bool shift, const char table[], const char tableShift[])
    {
        char specialKey = GetSpecialKey(scancode);
        if (specialKey != 0) 
            return 0;

        if (scancode < 0 || scancode >= 58) 
            return 0;

        if (shift) 
            return tableShift[scancode];

        return table[scancode];
    }
}

