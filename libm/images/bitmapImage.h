#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/files/defaultFile.h>

namespace ImageStuff
{
    struct BitmapImage
    {
        int32_t width, height, xOff, yOff;
        int64_t size;
        void* imageBuffer;
    };

    BitmapImage* ConvertBufferToBitmapImage(char* buffer, uint64_t size);
    BitmapImage* ConvertFileToBitmapImage(FileStuff::DefaultFile* file);
    
}