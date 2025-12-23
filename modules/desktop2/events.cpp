#include "events.h"
#include <cstdint>
#include <libm/syscallManager.h>
#include <libm/math.h>
#include <libm/heap/heap.h>
#include <libm/cstr.h>
// #include "main.h"

Queue<WindowBufferUpdatePacket*>* updateFramePackets;
List<Window*>* windowsUpdated;

void InitEvents() 
{
    updateFramePackets = new Queue<WindowBufferUpdatePacket*>(5);
    windowsUpdated = new List<Window*>(5);
}

void HandleEvents() 
{
    updateFramePackets->Clear();
    windowsUpdated->Clear();

    int msgCount = min(msgGetCount(), 50);
    serialPrint("> Updates: ");
    serialPrintLn(to_string(msgCount));
    for (int i = 0; i < msgCount; i++)
    {
        GenericMessagePacket* msg = msgGetMessage();
        serialPrint(" > Update: ");
        serialPrintLn(to_string((uint64_t)msg));
        if (msg == NULL)
            break;


        msg->Free();
        _Free(msg);
    }

    programWait(500);
}