#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef _KERNEL_SRC

#include "../kernel/osData/MStack/MStackM.h"
#include "../kernel/memory/heap.h"
#include "../kernel/interrupts/panic.h"
#include "../kernel/osData/MStack/MStackM.h"
#include "../kernel/devices/serial/serial.h"

#endif

#ifdef _KERNEL_MODULE

inline void Panic(const char* message, bool stop = false)
{
    asm("cli");
    asm("hlt");
}
inline void* _Malloc(uint64_t size, const char* name)
{
    return NULL;
}
inline void* _Malloc(uint64_t size)
{
    return NULL;
}

inline void _Free(void* ptr)
{

}

namespace Serial
{
    inline void Writelnf(const char* format, ...)
    {

    }
}

#define AddToStack()
#define RemoveFromStack()

#endif

#ifdef _USER_MODULE

inline void Panic(const char* message, bool stop = false)
{
    asm("cli");
    asm("hlt");
}
inline void* _Malloc(uint64_t size, const char* name)
{
    return NULL;
}
inline void* _Malloc(uint64_t size)
{
    return NULL;
}

inline void _Free(void* ptr)
{

}

namespace Serial
{
    inline void Writelnf(const char* format, ...)
    {

    }
}

#define AddToStack()
#define RemoveFromStack()
#endif