#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/experimental/RelocatableAllocator.h>
#include <libm/experimental/AutoFree.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>

char buffer[512];

int main()
{
    //globalCls();

    int argc = getArgC();
    char **argv = getArgV();
    ENV_DATA *env = getEnvData();

    globalPrintLn("Hello from a test (2) program!");
    
    int prio = programSetPriority(1);
    globalPrint("> Priority: ");
    globalPrintLn(to_string(prio));


    uint64_t pid = getPid();
    globalPrint("A> PID: ");
    globalPrintLn(to_string(pid));

    programWait(5000);

    int packetCount = 0;
        
    for (int i = 0; i < 5; i++)
    {
        packetCount = msgGetCount();
        globalPrint("A> Packet Count: ");
        globalPrintLn(to_string(packetCount));

        {
            globalPrintLn("A> Sending Message");
            GenericMessagePacket* packet = (GenericMessagePacket*)_Malloc(sizeof(GenericMessagePacket), "Test Packet");
            const char* str = "Hello from a test program!";
            *packet = GenericMessagePacket(MessagePacketType::GENERIC_DATA, (uint8_t*)str, StrLen(str)+1);
            msgSendMessage(packet, pid);
            packet->Free();
            _Free(packet);
        }

        packetCount = msgGetCount();
        globalPrint("A> Packet Count: ");
        globalPrintLn(to_string(packetCount));

        {
            GenericMessagePacket* packet = msgGetMessage();
            if (packet != NULL)
            {
                globalPrintLn("A> Got Message");
                globalPrint("A> Message: \"");
                globalPrint((char*)packet->Data);
                globalPrintLn("\"");
                packet->Free();
                _Free(packet);
            }
        }

        programWait(100);
    }

    packetCount = msgGetCount();
    globalPrint("A> Packet Count: ");
    globalPrintLn(to_string(packetCount));

    programWait(5000);

    globalPrintLn("> Done!");
    return 0;
    

    // globalPrintLn("> Goofy ah Scheduler Test:");

    // programWait(2000);
    // globalPrintLn("> USER ELF");
    // for (int i = 0; i < 150; i++)
    //     launchTestElfUser();
    // programWait(1000);

    // //return 0;
    // globalPrintLn("> KERNEL ELF");
    // for (int i = 0; i < 200; i++)
    //     launchTestElfKernel();
    // programWait(1000);
    // globalPrintLn("> Test Done!");
    
    // return 0;

    // for (int i = 0; i < 10;)
    // {
    //     if (serialCanReadChar())
    //     {
    //         char c = serialReadChar();
    //         serialPrintChar(c);
    //         globalPrintChar(c);
    //         i++;
    //     }
    //     else
    //         programWait(500);//programYield();
    // }
    return 0;
}
