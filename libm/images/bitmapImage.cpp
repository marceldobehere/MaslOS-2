#include "bitmapImage.h"
#include <libm/stubs.h>
#include <libm/syscallManager.h>
#include <libm/cstr.h>

namespace ImageStuff
{
    BitmapImage* ConvertFileToBitmapImage(FileStuff::DefaultFile* file)
    {
        return ConvertBufferToBitmapImage((char*)file->fileData, file->size);
    }

    BitmapImage* ConvertBufferToBitmapImage(char* data, uint64_t size)
    {
        AddToStack();
        if (data == NULL || size == 0)
        {
            RemoveFromStack();
            return NULL;
        }

        BitmapImage* image = (BitmapImage*)_Malloc(sizeof(BitmapImage), "Converting Default Kernel File to Image File");

        int32_t sizes[4] = {0, 0, 0, 0};

        for (int i = 0; i < 4; i++)
            sizes[i] = *((int32_t*)&data[i*4]);

        image->width = sizes[0];
        image->height = sizes[1];
        image->xOff = sizes[2];
        image->yOff = sizes[3];

        int64_t s2 = image->width * image->height;
        if (s2 < 0 || s2 >= size)
        {
            _Free(image);
            RemoveFromStack();
            return NULL;
        }

        image->size = *((int64_t*)&data[4*4]);

        image->imageBuffer = _Malloc(image->size, "Buffer for Image File");
        for (int i = 24; i < image->size+24; i++)
            ((char*)image->imageBuffer)[i-24] = data[i];

        RemoveFromStack();
        return image;
    }
}

