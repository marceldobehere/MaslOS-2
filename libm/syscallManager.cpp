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

uint64_t getParentPid()
{
    int syscall = SYSCALL_GET_PARENT_PID;
    uint64_t pid;

    asm("int $0x31" : "=a"(pid): "a"(syscall));
    return pid;
}

const char* getElfPath(uint64_t pid)
{
    int syscall = SYSCALL_GET_ELF_PATH_PID;
    const char* path;

    asm("int $0x31" : "=a"(path): "a"(syscall), "b"(pid));
    return path;

}

const char* getElfPath()
{
    int syscall = SYSCALL_GET_ELF_PATH;
    const char* path;

    asm("int $0x31" : "=a"(path): "a"(syscall));
    return path;
}

const char* getWorkingPath()
{
    int syscall = SYSCALL_GET_WORKING_PATH;
    const char* path;

    asm("int $0x31" : "=a"(path): "a"(syscall));
    return path;
}

bool pidExists(uint64_t pid)
{
    int syscall = SYSCALL_PID_EXISTS;
    bool exists;

    asm("int $0x31" : "=a"(exists): "a"(syscall), "b"(pid));
    return exists;
}

uint64_t envGetDesktopPid()
{
    int syscall = SYSCALL_ENV_GET_DESKTOP_PID;
    uint64_t pid;

    asm("int $0x31" : "=a"(pid): "a"(syscall));
    return pid;
}

uint64_t envGetStartMenuPid()
{
    int syscall = SYSCALL_ENV_GET_START_MENU_PID;
    uint64_t pid;

    asm("int $0x31" : "=a"(pid): "a"(syscall));
    return pid;
}


#include <libm/memStuff.h>
#ifdef _KERNEL_SRC
#include "../kernel/memory/heap.h"
#else
#include <libm/heap/heap.h>
#endif

MouseState* envGetMouseState()
{
    GenericMessagePacket* packet = new GenericMessagePacket(0, MessagePacketType::DESKTOP_GET_MOUSE_STATE);
    msgSendConv(packet, envGetDesktopPid());
    GenericMessagePacket* response = msgWaitConv(packet->ConvoID, 1000);
    packet->Free();
    _Free(packet);

    if (response == NULL)
        return NULL;
    if (response->Size != sizeof(MouseState))
    {
        response->Free();
        _Free(response);
        return NULL;
    }

    MouseState* mouseState = (MouseState*)_Malloc(sizeof(MouseState));
    _memcpy(response->Data, mouseState, sizeof(MouseState));

    response->Free();
    _Free(response);
    return mouseState;
}

bool envGetKeyState(int scancode)
{
    int syscall = SYSCALL_ENV_GET_KEY_STATE;
    bool state;

    asm("int $0x31" : "=a"(state): "a"(syscall), "b"(scancode));
    return state;
}

int envGetMouseSens()
{
    int syscall = SYSCALL_ENV_GET_MOUSE_SENS;
    int mouseSens;
    asm("int $0x31" : "=a"(mouseSens) : "a"(syscall));
    return mouseSens;
}

int envSetMouseSens(int sens)
{
    int syscall = SYSCALL_ENV_SET_MOUSE_SENS;
    int mouseSens;
    asm("int $0x31" : "=a"(mouseSens) : "a"(syscall), "b"(sens));
    return mouseSens;
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

        char* newAddr = (char*)((uint64_t)task->addrOfVirtPages + 0x1000 * task->requestedPages->GetCount());
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

void programWaitMsg()
{
    int syscall = SYSCALL_WAIT_MSG;
    asm("int $0x31" : : "a"(syscall));
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

uint64_t envGetTimeMicroS()
{
    int syscall = SYSCALL_ENV_GET_TIME_MICRO_S;
    uint64_t timeMicroS;
    asm("int $0x31" : "=a"(timeMicroS) : "a"(syscall));
    return timeMicroS;

}

uint64_t envGetTimeMs()
{
    int syscall = SYSCALL_ENV_GET_TIME_MS;
    uint64_t timeMs;
    asm("int $0x31" : "=a"(timeMs) : "a"(syscall));
    return timeMs;
}

RTC_Info* envGetTimeRTC()
{
    int syscall = SYSCALL_ENV_GET_TIME_RTC;
    RTC_Info* info;
    asm("int $0x31" : "=a"(info) : "a"(syscall));
    return info;
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

GenericMessagePacket* msgGetConv(uint64_t convoId)
{
    int syscall = SYSCALL_MSG_GET_MSG_CONVO;
    GenericMessagePacket* packet;
    asm("int $0x31" : "=a"(packet) : "a"(syscall), "b"(convoId));
    return packet;
}

#include <libm/cstr.h>

GenericMessagePacket* msgWaitConv(uint64_t convoId, uint64_t timeoutMs)
{
    int64_t endTime = envGetTimeMs() + timeoutMs;
    while ((int64_t)envGetTimeMs() < endTime)
    {
        GenericMessagePacket* packet = msgGetConv(convoId);
        if (packet != NULL)
            return packet;
        programYield();
    }

    return NULL;
}

#include <libm/rnd/rnd.h>

uint64_t msgSendConv(GenericMessagePacket* packet, uint64_t targetPid)
{
    uint64_t convoId = RND::RandomInt();
    return msgSendConv(packet, targetPid, convoId);
}

uint64_t msgSendConv(GenericMessagePacket* packet, uint64_t targetPid, uint64_t convoId)
{
    packet->ConvoID = convoId;
    msgSendMessage(packet, targetPid);
    return convoId;
}

uint64_t msgRespondConv(GenericMessagePacket* og, GenericMessagePacket* reply)
{
    reply->ConvoID = og->ConvoID;
    msgSendMessage(reply, og->FromPID);
    return og->ConvoID;
}

#include "stubs.h"


bool fsCreateFile(const char* path)
{
    int syscall = SYSCALL_FS_CREATE_FILE;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path));
    return success;
}

bool fsCreateFileWithSize(const char* path, uint64_t size)
{
    int syscall = SYSCALL_FS_CREATE_FILE_WITH_SIZE;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(size));
    return success;
}

bool fsCreateFolder(const char* path)
{
    int syscall = SYSCALL_FS_CREATE_FOLDER;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path));
    return success;
}


bool fsDeleteFile(const char* path)
{
    int syscall = SYSCALL_FS_DELETE_FILE;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path));
    return success;
}

bool fsDeleteFolder(const char* path)
{
    int syscall = SYSCALL_FS_DELETE_FOLDER;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path));
    return success;
}


bool fsRenameFile(const char* path, const char* newPath)
{
    int syscall = SYSCALL_FS_RENAME_FILE;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(newPath));
    return success;
}

bool fsRenameFolder(const char* path, const char* newPath)
{
    int syscall = SYSCALL_FS_RENAME_FOLDER;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(newPath));
    return success;
}


bool fsCopyFile(const char* path, const char* newPath)
{
    int syscall = SYSCALL_FS_COPY_FILE;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(newPath));
    return success;
}

bool fsCopyFolder(const char* path, const char* newPath)
{
    int syscall = SYSCALL_FS_COPY_FOLDER;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(newPath));
    return success;
}


bool fsFileExists(const char* path)
{
    int syscall = SYSCALL_FS_FILE_EXISTS;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path));
    return success;
}

bool fsFolderExists(const char* path)
{
    int syscall = SYSCALL_FS_FOLDER_EXISTS;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path));
    return success;
}


const char** fsGetFilesInPath(const char* path, uint64_t* count)
{
    int syscall = SYSCALL_FS_GET_FILES_IN_PATH;
    const char** files;
    uint64_t tCount;
    asm("int $0x31" : "=a"(files), "=b"(tCount) : "a"(syscall), "b"(path));
    *count = tCount;
    return files;
}

const char** fsGetFoldersInPath(const char* path, uint64_t* count)
{
    int syscall = SYSCALL_FS_GET_FOLDERS_IN_PATH;
    const char** folders;
    uint64_t tCount;
    asm("int $0x31" : "=a"(folders), "=b"(tCount) : "a"(syscall), "b"(path));
    *count = tCount;
    return folders;
}

const char** fsGetDrivesInRoot(uint64_t* count)
{
    int syscall = SYSCALL_FS_GET_DRIVES_IN_ROOT;
    const char** drives;
    uint64_t tCount;
    asm("int $0x31" : "=a"(drives), "=b"(tCount) : "a"(syscall));
    *count = tCount;
    return drives;
}


FsInt::FileInfo* fsGetFileInfo(const char* path)
{
    int syscall = SYSCALL_FS_GET_FILE_INFO;
    FsInt::FileInfo* info;
    asm("int $0x31" : "=a"(info) : "a"(syscall), "b"(path));
    return info;
}

FsInt::FolderInfo* fsGetFolderInfo(const char* path)
{
    int syscall = SYSCALL_FS_GET_FOLDER_INFO;
    FsInt::FolderInfo* info;
    asm("int $0x31" : "=a"(info) : "a"(syscall), "b"(path));
    return info;
}


bool fsReadFileIntoBuffer(const char* path, void* buffer, uint64_t start, uint64_t byteCount)
{
    int syscall = SYSCALL_FS_READ_FILE_INTO_BUFFER;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(buffer), "d"(start), "S"(byteCount));
    return success;
}

bool fsReadFileIntoBuffer(const char* path, void* buffer, uint64_t byteCount)
{
    return fsReadFileIntoBuffer(path, buffer, 0, byteCount);
}

bool fsWriteFileFromBuffer(const char* path, void* buffer, uint64_t byteCount)
{
    int syscall = SYSCALL_FS_WRITE_FILE_FROM_BUFFER;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(path), "c"(buffer), "d"(byteCount));
    return success;
}

#include <libm/memStuff.h>

bool fsReadFile(const char* path, void** buffer, uint64_t* byteCount)
{
    FsInt::FileInfo* info = fsGetFileInfo(path);
    if (info == NULL)
        return false;

    *byteCount = info->sizeInBytes;
    info->Destroy();
    *buffer = _Malloc(*byteCount);
    _memset(*buffer, 0, *byteCount);

    return fsReadFileIntoBuffer(path, *buffer, *byteCount); 
}



bool closeProcess(uint64_t pid)
{
    int syscall = SYSCALL_CLOSE_PROCESS;
    bool success;
    asm("int $0x31" : "=a"(success) : "a"(syscall), "b"(pid));
    return success;
}

uint64_t startProcess(const char* path, int argc, const char** argv, const char* workingDirectory)
{
    int syscall = SYSCALL_START_PROCESS;
    uint64_t pid;
    asm("int $0x31" : "=a"(pid) : "a"(syscall), "b"(path), "c"(argc), "d"(argv), "S"(workingDirectory));
    return pid;
}

uint64_t startFile(const char* path, const char* workingDirectory)
{
    int syscall = SYSCALL_START_FILE;
    uint64_t pid;
    asm("int $0x31" : "=a"(pid) : "a"(syscall), "b"(path), "c"(workingDirectory));
    return pid;
}