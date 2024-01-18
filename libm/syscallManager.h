#pragma once
#include <stdint.h>
#include <stddef.h>
#include "env/env.h"
#include "rtc/rtcInfo.h"

int getArgC();
char** getArgV();
ENV_DATA* getEnvData();
uint64_t getPid();
bool pidExists(uint64_t pid);
uint64_t getParentPid();
const char* getElfPath();
const char* getWorkingPath();

const char* getElfPath(uint64_t pid);

void* requestNextPage();
void* requestNextPages(int count);

void serialPrint(const char* str);
void serialPrintLn(const char* str);

void serialPrintChar(char c);
char serialReadChar();
bool serialCanReadChar();

void globalPrint(const char* str);
void globalPrintLn(const char* str);
void globalPrintChar(char chr);

void globalCls();

void programExit(int code);
void programCrash();
void programWait(int timeMs);
void programYield();
int programSetPriority(int priority);
void programWaitMsg();

uint64_t envGetTimeMs();
uint64_t envGetTimeMicroS();
uint64_t envGetDesktopPid();
uint64_t envGetStartMenuPid();
RTC_Info* envGetTimeRTC();

uint64_t randomUint64();

void launchTestElfUser();
void launchTestElfKernel();

#include "mouseState.h"

MouseState* envGetMouseState();
bool envGetKeyState(int scancode);

#include "msgPackets/msgPacket.h"

int msgGetCount();
GenericMessagePacket* msgGetMessage();
bool msgSendMessage(GenericMessagePacket* packet, uint64_t targetPid);


GenericMessagePacket* msgGetConv(uint64_t convoId);
GenericMessagePacket* msgWaitConv(uint64_t convoId, uint64_t timeoutMs);
uint64_t msgSendConv(GenericMessagePacket* packet, uint64_t targetPid, uint64_t convoId);
uint64_t msgSendConv(GenericMessagePacket* packet, uint64_t targetPid);
uint64_t msgRespondConv(GenericMessagePacket* og, GenericMessagePacket* reply);




#include <libm/fsStuff/fsInfo/fileSystemStructs.h>

bool fsCreateFile(const char* path);
bool fsCreateFileWithSize(const char* path, uint64_t size);
bool fsCreateFolder(const char* path);

bool fsDeleteFile(const char* path);
bool fsDeleteFolder(const char* path);

bool fsRenameFile(const char* path, const char* newPath);
bool fsRenameFolder(const char* path, const char* newPath);

bool fsCopyFile(const char* path, const char* newPath);
bool fsCopyFolder(const char* path, const char* newPath);

bool fsFileExists(const char* path);
bool fsFolderExists(const char* path);

const char** fsGetFilesInPath(const char* path, uint64_t* count);
const char** fsGetFoldersInPath(const char* path, uint64_t* count);
const char** fsGetDrivesInRoot(uint64_t* count);

FsInt::FileInfo* fsGetFileInfo(const char* path);
FsInt::FolderInfo* fsGetFolderInfo(const char* path);

bool fsReadFileIntoBuffer(const char* path, void* buffer, uint64_t start, uint64_t byteCount);

bool fsReadFileIntoBuffer(const char* path, void* buffer, uint64_t byteCount);
bool fsWriteFileFromBuffer(const char* path, void* buffer, uint64_t byteCount);

bool fsReadFile(const char* path, void** buffer, uint64_t* byteCount);





bool closeProcess(uint64_t pid);
uint64_t startProcess(const char* path, int argc, const char** argv, const char* workingDirectory);
uint64_t startFile(const char* path, const char* workingDirectory);