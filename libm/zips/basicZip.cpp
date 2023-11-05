#include "basicZip.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>

namespace ZipStuff
{
    namespace ZIP
    {
        FileStuff::DefaultFile* GetFileFromFileName(ZIPFile* zip, const char* filename)
        {
            AddToStack();
            if (zip == NULL)
            {
                RemoveFromStack();
                return NULL;
            }
            RemoveFromStack();




            AddToStack();

            //GlobalRenderer->Println("                                     NAME ADDR: {}", ConvertHexToString((uint64_t)filename), Colors.yellow);
            //GlobalRenderer->Println("                                     NAME: \"{}\"", filename, Colors.yellow);
            //GlobalRenderer->Println("                                     ZIP FILE COUNT: {}", to_string(zip->fileCount), Colors.yellow);

            for (int i = 0; i < zip->fileCount; i++)
            {
                AddToStack();
                //GlobalRenderer->Println("                                     NAME ADDR: {}", ConvertHexToString((uint64_t)zip->files[i].filename), Colors.yellow);
                //GlobalRenderer->Println("                                     NAME: \"{}\"", zip->files[i].filename, Colors.yellow);
                RemoveFromStack();

                AddToStack();
                if (StrEquals(zip->files[i].filename, filename))
                {
                    RemoveFromStack();
                    RemoveFromStack();
                    return &zip->files[i];
                }
                RemoveFromStack();
            }

            RemoveFromStack();
            return NULL;
        }

        ZIPFile* GetZIPFromDefaultFile(FileStuff::DefaultFile* file)
        {
            return GetZIPFromBuffer(file->fileData, file->size);
        }

        ZIPFile* GetZIPFromBuffer(void* buffer, uint64_t size)
        {
            if (buffer == NULL)
                return NULL;

            ZIPFile* zip = (ZIPFile*)_Malloc(sizeof(ZIPFile));

            zip->size = *((uint64_t*)((uint64_t)buffer + 0));
            zip->fileCount = *((uint32_t*)((uint64_t)buffer + 8));
            zip->files = (FileStuff::DefaultFile*)_Malloc(sizeof(FileStuff::DefaultFile) * zip->fileCount);
            uint64_t tOffset = 12;
            for (int i = 0; i < zip->fileCount; i++)
            {
                zip->files[i].filenameSize = *((uint32_t*)((uint64_t)buffer + tOffset));
                tOffset += 4;

                zip->files[i].filename = (char*)_Malloc(zip->files[i].filenameSize + 1);
                for (int x = 0; x < zip->files[i].filenameSize; x++)
                    zip->files[i].filename[x] = *((char*)((uint64_t)buffer + tOffset++));
                zip->files[i].filename[zip->files[i].filenameSize] = 0;
            

                zip->files[i].size = *((uint64_t*)((uint64_t)buffer + tOffset));
                
                tOffset += 8;

                zip->files[i].fileData = (void*)((uint64_t)buffer + tOffset);
                tOffset += zip->files[i].size;

            }

            return zip;
        }
    }

    

}