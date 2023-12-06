#include "stdio.h"
#include <libm/syscallManager.h>
#include <libm/msgPackets/msgPacket.h>

namespace STDIO
{
    StdioInst parent;
    void initStdio(bool needLoggerWindow)
    {
        // try to connect with parent
        




    }

    StdioInst initStdio(uint64_t pid)
    {
        StdioInst other;
        other.pid = pid;
        other.convoId = NULL;



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
        // TODO: actually create and send packet
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

    // Read from any
    int read(StdioInst other)
    {
        // TODO: actually check for received packets
        return -1;
    }
};
