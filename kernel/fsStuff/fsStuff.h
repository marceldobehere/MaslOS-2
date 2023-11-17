#pragma once

#include "../diskStuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../diskStuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../diskStuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../diskStuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
// #include "../tasks/readFile/taskReadFile.h"
// #include "../tasks/writeFile/taskWriteFile.h"

namespace FS_STUFF
{
    struct FileBuffer
    {
        char* buffer;
        uint64_t bufferLen;
    };

    bool CreateFileIfNotExist(const char* path);
    bool FileExists(const char* path);

    bool ReadFileFromFullPath(const char* path, char** resBuffer, int* resBufferLen);
    bool WriteFileToFullPath(const char* path, char* buffer, int bufferLen, bool createIfNotExists);

    // TaskReadFile* ReadFileTask(const char* path);
    // TaskWriteFile* WriteFileTask(const char* path, char* buffer, int bufferLen, bool createIfNotExists);

    FilesystemInterface::GenericFilesystemInterface* GetFsInterfaceFromFullPath(const char* path);
    FilesystemInterface::FileInfo* GetFileInfoFromFullPath(const char* path);
    FilesystemInterface::FolderInfo* GetFolderInfoFromFullPath(const char* path);

    //bool OpenFile(const char* path);
    //void RunTerminalCommand(const char* terminalCommand, const char* terminalWindowTitle, bool hideTerminalWindow, bool autoCloseTerminalWindow);

    

}
