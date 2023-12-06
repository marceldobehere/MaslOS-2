#include <libm/syscallManager.h>
#include <libm/cstrTools.h>

int main(int argc, char** argv)
{
    if (argc != 1 || argv == NULL)
        return -1;
    const char* path = argv[0];
    if (!StrEndsWith(path, ".elf"))
        return -1;

    startProcess(path, 0, NULL, getWorkingPath());
    return 0;
}
