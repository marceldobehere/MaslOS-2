#include "syscallManager.h"
#include "syscallList.h"

int getArgC()
{
    int syscall = SYSCALL_GET_ARGC;
    int argc;

    asm("int $0x31" : "=a"(argc): "a"(syscall));
    return argc;
}

char** getArgV()
{
    int syscall = SYSCALL_GET_ARGV;
    char** argv;

    asm("int $0x31" : "=a"(argv): "a"(syscall));
    return argv;
}

ENV_DATA* getEnvData()
{
    int syscall = SYSCALL_GET_ENV;
    ENV_DATA* env;

    asm("int $0x31" : "=a"(env): "a"(syscall));
    return env;
}