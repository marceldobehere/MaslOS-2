#pragma once
#include <stdint.h>
#include <stddef.h>

namespace STDIO
{
    const uint64_t STDIO_INIT_CONVO_ID = 0xFF1234791487234;

    struct StdioInst
    {
        uint64_t pid;
        uint64_t convoId;
    };

    extern StdioInst parent;
    void initStdio(bool needLoggerWindow);
    StdioInst initStdio(uint64_t pid);


    // Print to parent
    void print(char chr);
    void print(const char* str);
    void println();
    void println(char chr);
    void println(const char* str);

    // Print to any
    void print(char chr, StdioInst other);
    void print(const char* str, StdioInst other);
    void println(StdioInst other);
    void println(char chr, StdioInst other);
    void println(const char* str, StdioInst other);


    // Read from parent
    int read(); // returns a char or -1 if there is no data

    // Read from any
    int read(StdioInst other);
};
