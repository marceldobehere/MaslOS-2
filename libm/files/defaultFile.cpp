#include "defaultFile.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>

namespace FileStuff
{
    DefaultFile* GetDefaultFileFromBuffer(const char* path, void* buffer, uint64_t size)
    {
        AddToStack();
        DefaultFile* file = (DefaultFile*)_Malloc(sizeof(DefaultFile));
        file->size = size;
        file->fileData = buffer;
        file->filename = StrCopy(path);
        file->filenameSize = StrLen(path);
        RemoveFromStack();
        return file;
    }
}