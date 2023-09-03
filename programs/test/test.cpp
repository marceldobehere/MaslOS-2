#include "test.h"
#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/RelocatableAllocator.h>
#include <libm/AutoFree.h>

char buffer[512];

int main()
{
    globalCls();
    int argc = getArgC();
    char **argv = getArgV();
    ENV_DATA *env = getEnvData();
    // AutoFree<char> chptr('a');

    return 0;
}
