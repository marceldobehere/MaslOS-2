#pragma once
#include <stdint.h>
#include <stddef.h>

namespace STDIO
{
    struct OtherStdio
    {
        uint64_t pid;
        uint64_t convoId;
    };

    extern OtherStdio parent;
    void initStdio();

    // Print to parent
    void print(char chr);
    void print(const char* str);
    void println();
    void println(char chr);
    void println(const char* str);

    // Print to any
    void print(char chr, OtherStdio other);
    void print(const char* str, OtherStdio other);
    void println(OtherStdio other);
    void println(char chr, OtherStdio other);
    void println(const char* str, OtherStdio other);


    // Read from parent
    int read(); // returns a char or -1 if there is no data

    // Read from any
    int read(OtherStdio other);
};
