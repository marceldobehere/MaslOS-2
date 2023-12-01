#include "stdio.h"

namespace STDIO
{
    OtherStdio parent;
    void initStdio()
    {
        // TODO: Get parent pid and convo id
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
    void print(const char* str, OtherStdio other)
    {
        // TODO: actually create and send packet
    }
    
    void print(char chr, OtherStdio other)
    {
        char str[2];
        str[0] = chr;
        str[1] = '\0';
        print(str, other);
    }
    void println(OtherStdio other)
    {
        print("\n\r", other);
    }
    void println(char chr, OtherStdio other)
    {
        print(chr, other);
        println(other);
    }
    void println(const char* str, OtherStdio other)
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
    int read(OtherStdio other)
    {
        // TODO: actually check for received packets
        return -1;
    }
};
