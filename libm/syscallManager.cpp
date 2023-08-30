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

void serialPrint(const char* str)
{
    int syscall = SYSCALL_SERIAL_PRINT;
    asm("int $0x31" : : "a"(syscall), "b"(str));
}

void serialPrintLn(const char* str)
{
    int syscall = SYSCALL_SERIAL_PRINTLN;
    asm("int $0x31" : : "a"(syscall), "b"(str));
}

void globalPrint(const char* str)
{
    int syscall = SYSCALL_GLOBAL_PRINT;
    asm("int $0x31" : : "a"(syscall), "b"(str));
}

void globalPrintLn(const char* str)
{
    int syscall = SYSCALL_GLOBAL_PRINTLN;
    asm("int $0x31" : : "a"(syscall), "b"(str));
}

void globalCls()
{
    int syscall = SYSCALL_GLOBAL_CLS;
    asm("int $0x31" : : "a"(syscall));
}

void exitProgram(int code)
{
    int syscall = SYSCALL_EXIT;
    asm("int $0x31" : : "a"(syscall), "b"(code));
}