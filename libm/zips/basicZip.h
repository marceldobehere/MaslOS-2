#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/files/defaultFile.h>

namespace ZipStuff
{
	typedef struct
	{
        int64_t size;
        int32_t fileCount;
        
        FileStuff::DefaultFile* files;
	} ZIPFile;

    namespace ZIP
    {
        FileStuff::DefaultFile* GetFileFromFileName(ZIPFile* zip, const char* filename);
		ZIPFile* GetZIPFromDefaultFile(FileStuff::DefaultFile* file);
        ZIPFile* GetZIPFromBuffer(void* buffer, uint64_t size);
    }
}

