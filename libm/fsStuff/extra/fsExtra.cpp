#include "fsExtra.h"
#include <libm/stubs.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>

namespace FS_EXTRA
{
        char* GetDriveNameFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        AddToStack();
        int fIndex = -1;
        int len = StrLen(path);
        RemoveFromStack();
        
        AddToStack();
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        RemoveFromStack();

        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        int nLen = fIndex;
        char* name = (char*)_Malloc(nLen + 1, "drive name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }

    char* GetFilePathFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        int fIndex = -1;
        int len = StrLen(path);
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        // AB :  XYZ (6)
        // 01 2 345
        // 6 - (2 + 1) => 3

        int nLen = len - (fIndex + 1);
        char* name = (char*)_Malloc(nLen + 1, "path name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[fIndex + 1 + i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }
    
    char* GetFolderPathFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        int fIndex = -1;
        int len = StrLen(path);
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        int lSI = -1;
        for (int i = len - 1; i > 0; i--)
            if (path[i] == '/')
            {
                lSI = i;
                break;
            }


        char* name;


        // AB :  XYZ (6)
        // 01 2 345
        // 6 - (2 + 1) => 3



        if (lSI == -1)
        {
            lSI = fIndex + 1;
        }

        int nLen = lSI - (fIndex + 1);
        name = (char*)_Malloc(nLen + 1, "path name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[fIndex + 1 + i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }
}