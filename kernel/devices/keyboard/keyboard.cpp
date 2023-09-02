#include "keyboard.h"
#include "scancodeTranslation.h"
#include <libm/queue/queue_basics.h>
#include <libm/lock/lock.h>

namespace Keyboard
{
    bool KeyboardScancodeState[KeyboardScancodeStateSize];
    Lockable<Queue<int>*> keyboardQueue = NULL;
    int SubScancodeStep = 0;
    int SubScancodeMode = 0;
    uint8_t SubScancodes[4];

    void InitKeyboard()
    {
        for (int i = 0; i < KeyboardScancodeStateSize; i++)
            KeyboardScancodeState[i] = false;

        SubScancodeStep = 0;
        SubScancodeMode = 0;
        for (int i = 0; i < 4; i++)
            SubScancodes[i] = 0;

        keyboardQueue = Lockable<Queue<int>*>(new Queue<int>(4));
    }

    
    void HandleKeyboardInterrupt(uint8_t scancode)
    {
        if (!keyboardQueue.HasItem())
            return;

        if (scancode == 0xE0)
        {
            SubScancodeMode = 1;
            SubScancodeStep = 0;
            return;
        }
        else if (scancode == 0xE1)
        {
            SubScancodeMode = 2;
            SubScancodeStep = 0;
            return;
        }
        else if (SubScancodeMode != 0)
        {
            SubScancodes[SubScancodeStep] = (scancode & ~KEY_RELEASED);
            SubScancodeStep++;

            if (SubScancodeStep >= SubScancodeMode || SubScancodeMode > 4)
            {
                bool released = (scancode & KEY_RELEASED) != 0;
                HandleMultiScanCodeKey(released);
                SubScancodeStep = 0;
                SubScancodeMode = 0;
                return;
            }
        }
        
        keyboardQueue.Lock();
        keyboardQueue.obj->Enqueue(scancode);
        keyboardQueue.Unlock();
    }

    void HandleMultiScanCodeKey(bool released)
    {
        int finalKey = 0;

        if (SubScancodeMode == 1)
        {
            if (SubScancodes[0] == 0x1D)
                finalKey = Key_RightControl;
            else if (SubScancodes[0] == 0x38)
                finalKey = Key_RightAlt;
        }
        else if (SubScancodeMode == 2)
        {
            
        }

        if (finalKey == 0)
            return;

        finalKey |= KEY_RELEASED;

        keyboardQueue.Lock();
        keyboardQueue.obj->Enqueue(finalKey);
        keyboardQueue.Unlock();
    }

    bool KeyAvaiable()
    {
        if (!keyboardQueue.HasItem())
            return;
        
        keyboardQueue.Lock();
        bool result = keyboardQueue.obj->GetCount() > 0;
        keyboardQueue.Unlock();

        return result;
    }

    uint8_t GetKey()
    {
        if (!KeyAvaiable())
            return 0;

        keyboardQueue.Lock();
        uint8_t scancode = keyboardQueue.obj->Dequeue();
        keyboardQueue.Unlock();

        uint8_t actualScancode = scancode & ~KEY_RELEASED;

        if ((scancode & KEY_RELEASED) == 0)
            KeyboardScancodeState[actualScancode] = true;
        else
            KeyboardScancodeState[actualScancode] = false;
    }

    bool IsKeyPressed(int scancode)
    {
        if (scancode == Key_GeneralShift)
            return KeyboardScancodeState[Key_LeftShift] || KeyboardScancodeState[Key_RightShift];

        if (scancode == Key_GeneralControl)
            return KeyboardScancodeState[Key_LeftControl] || KeyboardScancodeState[Key_RightControl];

        if (scancode == Key_GeneralAlt)
            return KeyboardScancodeState[Key_LeftAlt] || KeyboardScancodeState[Key_RightAlt];



        if (scancode >= 0 && scancode < KeyboardScancodeStateSize)
            return KeyboardScancodeState[scancode];

        return false;
    }
}