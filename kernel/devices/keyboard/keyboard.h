#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/keyboard.h>

namespace Keyboard
{
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
