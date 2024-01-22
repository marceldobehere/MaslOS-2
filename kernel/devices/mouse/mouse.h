#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/math.h>
#include <libm/queue/queue_mousePacket.h>
#include <libm/lock/lock.h>

namespace Mouse
{
    //extern bool clicks[3];
    //extern MPoint MousePosition;

    extern int mouseCycleSkip;

    extern Lockable<Queue<MousePacket>*> mousePackets;

    void Mousewait();

    void MousewaitInput();

    void MouseWrite(uint8_t value);

    uint8_t MouseRead();

    void InitPS2Mouse();

    void MouseWrite(uint8_t value);

    void HandlePS2Mouse(uint8_t data);

    int MousePacketsAvailable();
    struct MiniMousePacket
    {
        bool Valid;
        int X;
        int Y;

        bool LeftButton;
        bool RightButton;
        bool MiddleButton;
    };

    const MiniMousePacket InvalidMousePacket = {false, 0, 0, false, false, false};

    MiniMousePacket ProcessMousePacket(MousePacket packet);
    void ProcessMousePackets(int limit);
    void ProcessMousePackets();
}

