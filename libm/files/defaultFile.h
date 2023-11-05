#pragma once
#include <stdint.h>

namespace FileStuff
{
	typedef struct
	{
        int64_t size;
        int32_t filenameSize;
        
        char* filename;
        void* fileData;
	} DefaultFile;

    DefaultFile* GetDefaultFileFromBuffer(const char* path, void* buffer, uint64_t size);
}