#include "stdio.h"
#include <libm/syscallManager.h>
#include <libm/msgPackets/msgPacket.h>
#include <libm/rnd/rnd.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>

namespace STDIO
{
    StdioInst parent;

    void initStdio(bool needLoggerWindow)
    {
        parent.pid = getParentPid();

        // try to connect with parent
        GenericMessagePacket* packet = msgWaitConv(STDIO_INIT_CONVO_ID, 3000);

        if (packet != NULL && packet->Type == MessagePacketType::GENERIC_DATA && packet->Size == 8 && packet->FromPID == parent.pid)
        {
            parent.convoId = *(uint64_t*)packet->Data;

            packet->Free();
            _Free(packet);
        }
        else
        {
            if (packet != NULL)
            {
                packet->Free();
                _Free(packet);
            }

            // failed to connect with parent
            if (needLoggerWindow)
            {
                // TODO: create logger window and connect to that instead
                Panic("NO LOGGER WINDOW IMLPEMENTED", true);
            }
        }
    }

    StdioInst initStdio(uint64_t pid)
    {
        StdioInst other;
        other.pid = pid;
        other.convoId = RND::RandomInt();

        {
            GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::GENERIC_DATA, (uint8_t*)&other.convoId, 8);
            msgSendConv(packet, other.pid, STDIO_INIT_CONVO_ID);
            packet->Free();
            _Free(packet);  
        }

        return other;
    }

    // Print to parent
    void print(const char* str)
    {
        print(str, parent);
    }

    void print(char chr)
    {
        char str[2];
        str[0] = chr;
        str[1] = '\0';
        print(str);
    }
    void println()
    {
        print("\n\r");
    }
    void println(char chr)
    {
        print(chr);
        println();
    }
    void println(const char* str)
    {
        print(str);
        println();
    }

    // Print to any
    void print(const char* str, StdioInst other)
    {
        GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::GENERIC_DATA, (uint8_t*)str, StrLen(str + 1));
        msgSendConv(packet, other.pid, other.convoId);
        packet->Free();
        _Free(packet);
    }
    
    void print(char chr, StdioInst other)
    {
        char str[2];
        str[0] = chr;
        str[1] = '\0';
        print(str, other);
    }
    void println(StdioInst other)
    {
        print("\n\r", other);
    }
    void println(char chr, StdioInst other)
    {
        print(chr, other);
        println(other);
    }
    void println(const char* str, StdioInst other)
    {
        print(str, other);
        println(other);
    }


    // Read from parent
    int read()
    {
        return read(parent);
    }

    const char* tStr = NULL;
    int tStrCount = 0;

    // Read from any
    int read(StdioInst other)
    {
        while (tStr == NULL)
        {
            GenericMessagePacket* packet = msgGetConv(other.convoId);
            if (packet == NULL)
                return -1;
            
            if (packet->Type != MessagePacketType::GENERIC_DATA)
            {
                packet->Free();
                _Free(packet);
                return -1;
            }

            tStr = (const char*)StrCopy((const char*)packet->Data);
            tStrCount = 0;
            packet->Free();
            _Free(packet);

            if (StrLen(tStr) == 0)
            {
                _Free(tStr);
                tStr = NULL;
            }
        }
        
        if (tStrCount == StrLen(tStr))
        {
            tStr = NULL;
            tStrCount = 0;
            return -1;
        }
        else
            return tStr[tStrCount++];
    }
};
