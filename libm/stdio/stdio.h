#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/queue/queue_basics.h>

namespace STDIO
{
    const uint64_t STDIO_INIT_CONVO_ID = 0xFF1234791487234;

    struct StdioInst
    {
        uint64_t pid;
        uint64_t convoId;
        Queue<uint8_t>* readQueue = NULL;

        StdioInst(uint64_t pid);
        void Free();
    };

    extern StdioInst* parent;
    void initStdio(bool needLoggerWindow);
    StdioInst* initStdio(uint64_t pid);
    StdioInst* initStdioClient(uint64_t pid);


    // Print to parent
    void print(char chr);
    void print(const char* str);
    void println();
    void println(char chr);
    void println(const char* str);
    void printlnf(const char* str, ...);
    void printf(const char* str, ...);
    
    // Print to any
    void print(char chr, StdioInst* other);
    void print(const char* str, StdioInst* other);
    void println(StdioInst* other);
    void println(char chr, StdioInst* other);
    void println(const char* str, StdioInst* other);
    void printlnf(StdioInst* other, const char* str, ...);
    void printf(StdioInst* other, const char* str, ...);

    // Clear parent
    void clear();

    // Clear any
    void clear(StdioInst* other);


    // Read from parent
    int read(); // returns a char or -1 if there is no data
    bool available();
    int bytesAvailable();

    // Read from any
    int read(StdioInst* other);
    bool available(StdioInst* other);
    int bytesAvailable(StdioInst* other);


    // Send bytes to parent
    void sendBytes(uint8_t* bytes, uint64_t size);

    // Send bytes to any
    void sendBytes(uint8_t* bytes, uint64_t size, StdioInst* other);


    // Read bytes from parent
    uint64_t readBytes(uint8_t* bytes, uint64_t size);

    // Read bytes from any
    uint64_t readBytes(uint8_t* bytes, uint64_t size, StdioInst* other);

    // Read line from parent
    const char* readLine();

    // Read line from any
    const char* readLine(StdioInst* other);
};
