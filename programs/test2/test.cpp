#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/experimental/RelocatableAllocator.h>
#include <libm/experimental/AutoFree.h>

char buffer[512];

int main()
{
    globalCls();

    int argc = getArgC();
    char **argv = getArgV();
    ENV_DATA *env = getEnvData();

    globalPrintLn("Hello from a test (2) program!");
    for (int i = 0; i < 10;)
    {
        if (serialCanReadChar())
        {
            char c = serialReadChar();
            serialPrintChar(c);
            globalPrintChar(c);
            i++;
        }
    }
    return 0;
}
