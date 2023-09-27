#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Keyboard
{
    #define KEY_RELEASED 0x80

    #define Key_LeftShift      0x2A
    #define Key_RightShift     0x36
    #define Key_GeneralShift  0x0101 // special key thats not a normal scancode

    #define Key_Enter          0x1C
    #define Key_Escape        0x01

    #define Key_LeftControl   0x1D
    #define Key_RightControl  0x0102 // special key
    #define Key_GeneralControl 0x0103 // special key

    #define Key_Tab            0x0F
    #define Key_Backspace    0x0E

    #define Key_ArrowUp      0x48
    #define Key_ArrowDown  0x50
    #define Key_ArrowLeft   0x4B
    #define Key_ArrowRight   0x4D

    #define Key_LeftAlt       0x38
    #define Key_RightAlt      0x0104 // special key
    #define Key_GeneralAlt   0x0105 // special key

    #define Key_F1           0x3B 
    #define Key_F2           0x3C
    #define Key_F3           0x3D
    #define Key_F4           0x3E
    #define Key_F5           0x3F
    #define Key_F6           0x40
    #define Key_F7           0x41
    #define Key_F8           0x42
    #define Key_F9           0x43
    #define Key_F10         0x44
    #define Key_F11         0x57
    #define Key_F12         0x58





    const int KeyboardScancodeStateSize = 512;
    extern bool KeyboardScancodeState[KeyboardScancodeStateSize];
    extern int SubScancodeStep;
    extern int SubScancodeMode;
    extern uint8_t SubScancodes[4];

    void InitKeyboard();

    void HandleKeyboardInterrupt(uint8_t scancode);
    void HandleMultiScanCodeKey(bool released);

    int KeysAvaiable();
    bool DoKey();

    struct MiniKeyInfo
    {
        bool IsPressed;
        int Scancode;
        char AsciiChar;
    };
    const MiniKeyInfo NoKey = { false, 0, 0 };
    MiniKeyInfo DoAndGetKey();

    bool IsKeyPressed(int scancode);
}
