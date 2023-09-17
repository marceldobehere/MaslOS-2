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

uint64_t getPid()
{
    int syscall = SYSCALL_GET_PID;
    uint64_t pid;

    asm("int $0x31" : "=a"(pid): "a"(syscall));
    return pid;
}

#ifdef _KERNEL_SRC
#include "../kernel/paging/PageTableManager.h"
#include "../kernel/scheduler/scheduler.h"
#include "osTask.h"
#include "../kernel/devices/serial/serial.h"
#include "../kernel/interrupts/panic.h"
#endif

void* requestNextPage()
{
    return requestNextPages(1);
}

void* requestNextPages(int count)
{
    #ifdef _KERNEL_SRC
        int pageCount = count;
        osTask* task = Scheduler::CurrentRunningTask;
        PageTableManager manager = PageTableManager((PageTable*)task->pageTableContext);

        char* newAddr = (char*)(MEM_AREA_USER_PROGRAM_REQUEST_START + 0x1000 * task->requestedPages->GetCount());
        void* resAddr = (void*)newAddr;

        for (int i = 0; i < pageCount; i++)
        {
            void* tempPage = GlobalAllocator->RequestPage();
            task->requestedPages->Add(tempPage);
            
            manager.MapMemory((void*)newAddr, (void*)tempPage, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
            GlobalPageTableManager.MapMemory((void*)newAddr, (void*)tempPage, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);

            newAddr += 0x1000;
        }

        Serial::Writelnf("K> Requested next %d pages to %X", pageCount, newAddr);
        return resAddr;
        
        // osTask* task = Scheduler::CurrentRunningTask;
        // if (task == NULL)
        //     Panic("REQUESTING PAGE BUT TASK IS NULL", true);
        // void* tempPage = GlobalAllocator->RequestPage();
        // int count = task->requestedPages->GetCount();

        // task->requestedPages->Add(tempPage);
        
        // void* newAddr = (void*)(MEM_AREA_USER_PROGRAM_REQUEST_START + 0x1000 * count);
        // PageTableManager manager = PageTableManager((PageTable*)task->pageTableContext);
        // manager.MapMemory(newAddr, (void*)tempPage, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
        // GlobalPageTableManager.MapMemory(newAddr, (void*)tempPage, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);

        // Serial::Writelnf("> KERNEL Requested page for task: %X", newAddr);

        // return newAddr;
    #else
        int syscall = SYSCALL_REQUEST_NEXT_PAGES;
        void* page;

        asm("int $0x31" : "=a"(page): "a"(syscall), "b"(count));
        return page;
    #endif
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

char serialReadChar()
{
    int syscall = SYSCALL_SERIAL_READ_CHAR;
    char ch;
    asm("int $0x31" : "=a"(ch) : "a"(syscall));
    return ch;
}

bool serialCanReadChar()
{
    int syscall = SYSCALL_SERIAL_CAN_READ_CHAR;
    bool canRead;
    asm("int $0x31" : "=a"(canRead) : "a"(syscall));
    return canRead;
}

void serialPrintChar(char c)
{
    int syscall = SYSCALL_SERIAL_PRINT_CHAR;
    asm("int $0x31" : : "a"(syscall), "b"(&c));
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

void globalPrintChar(char chr)
{
    int syscall = SYSCALL_GLOBAL_PRINT_CHAR;
    asm("int $0x31" : : "a"(syscall), "b"(chr));
}


void globalCls()
{
    int syscall = SYSCALL_GLOBAL_CLS;
    asm("int $0x31" : : "a"(syscall));
}

void programExit(int code)
{
    int syscall = SYSCALL_EXIT;
    asm("int $0x31" : : "a"(syscall), "b"(code));
}

void programCrash()
{
    int syscall = SYSCALL_CRASH;
    asm("int $0x31" : : "a"(syscall));
}

void programWait(int timeMs)
{
    int syscall = SYSCALL_WAIT;
    asm("int $0x31" : : "a"(syscall), "b"(timeMs));
}

void programYield()
{
    int syscall = SYSCALL_YIELD;
    asm("int $0x31" : : "a"(syscall));
}

int programSetPriority(int priority)
{
    int actualPrio = 0;
    int syscall = SYSCALL_SET_PRIORITY;
    asm("int $0x31" : "=a"(actualPrio) : "a"(syscall), "b"(priority));
    return actualPrio;
}

uint64_t envGetTimeMs()
{
    int syscall = SYSCALL_ENV_GET_TIME_MS;
    uint64_t timeMs;
    asm("int $0x31" : "=a"(timeMs) : "a"(syscall));
    return timeMs;
}

uint64_t randomUint64()
{
    int syscall = SYSCALL_RNG_UINT64;
    uint64_t rand;
    asm("int $0x31" : "=a"(rand) : "a"(syscall));
    return rand;
}

void launchTestElfUser()
{
    int syscall = SYSCALL_LAUNCH_TEST_ELF_USER;
    asm("int $0x31" : : "a"(syscall));
}

void launchTestElfKernel()
{
    int syscall = SYSCALL_LAUNCH_TEST_ELF_KERNEL;
    asm("int $0x31" : : "a"(syscall));
}

int msgGetCount()
{
    int syscall = SYSCALL_MSG_GET_COUNT;
    int count;
    asm("int $0x31" : "=a"(count) : "a"(syscall));
    return count;
}

GenericMessagePacket* msgGetMessage()
{
    int syscall = SYSCALL_MSG_GET_MSG;
    GenericMessagePacket* packet;
    asm("int $0x31" : "=a"(packet) : "a"(syscall));
    return packet;
}

bool msgSendMessage(GenericMessagePacket* packet, uint64_t targetPid)
{
    int syscall = SYSCALL_MSG_SEND_MSG;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(packet), "c"(targetPid));
    return success;
}