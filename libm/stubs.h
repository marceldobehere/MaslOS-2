#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef _KERNEL_SRC

#include "../kernel/osData/MStack/MStackM.h"
#include "../kernel/memory/heap.h"
#include "../kernel/interrupts/panic.h"
#include "../kernel/osData/MStack/MStackM.h"
#include "../kernel/devices/serial/serial.h"
#include "../kernel/devices/pit/pit.h"

#endif

#ifdef _KERNEL_MODULE

#include "heap/heap.h"
#include "syscallManager.h"

#define AddToStack()
#define RemoveFromStack()

inline void Panic(const char* message, bool stop = false)
{
    serialPrintLn("PROGRAM PANIC AAAA");
    serialPrintLn(message);
    programCrash();
}

namespace Serial
{
    inline void Writelnf(const char* format, ...)
    {
        // cant format shit yet so gg
        serialPrintLn(format);
    }
}

namespace PIT
{
    inline uint64_t TimeSinceBootMS()
    {
        return envGetTimeMs();
    }
}
#endif



#ifdef _USER_MODULE

#include "heap/heap.h"
#include "syscallManager.h"

#define AddToStack()
#define RemoveFromStack()

inline void Panic(const char* message, bool stop = false)
{
    serialPrintLn("PROGRAM PANIC AAAA");
    serialPrintLn(message);
    programCrash();
}

namespace Serial
{
    inline void Writelnf(const char* format, ...)
    {
        // cant format shit yet so gg
        serialPrintLn(format);
    }
}

namespace PIT
{
    inline uint64_t TimeSinceBootMS()
    {
        return envGetTimeMs();
    }
}
#endif