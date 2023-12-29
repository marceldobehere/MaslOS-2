#include "dialogStuff.h"
#include <libm/stdio/stdio.h>
#include <libm/syscallManager.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>

using namespace STDIO;

namespace Dialog
{
    const char* OpenFileDialog()
    {
        return OpenFileDialog(NULL);
    }
    
    const char* OpenFileDialog(const char* path)
    {
        uint64_t newPid;
        if (path != NULL)
            newPid = startProcess("bruh:programs/openFileExplorer/openFileExplorer.elf", 1, &path, "");
        else
            newPid = startProcess("bruh:programs/openFileExplorer/openFileExplorer.elf", 0, NULL, "");

        if (newPid == 0)
            return NULL;

        StdioInst* tempStdio = initStdio(newPid);
        const char* res = readLine(tempStdio);

        if (res != NULL && StrEquals(res, ""))
        {
            _Free(res);
            res = NULL;
        }

        tempStdio->Free();
        _Free(tempStdio);

        return res;
    }

    const char* SaveFileDialog()
    {
        return SaveFileDialog(NULL);
    }

    const char* SaveFileDialog(const char* path)
    {
        uint64_t newPid;
        if (path != NULL)
            newPid = startProcess("bruh:programs/saveFileExplorer/saveFileExplorer.elf", 1, &path, "");
        else
            newPid = startProcess("bruh:programs/saveFileExplorer/saveFileExplorer.elf", 0, NULL, "");

        if (newPid == 0)
            return NULL;

        StdioInst* tempStdio = initStdio(newPid);
        const char* res = readLine(tempStdio);

        if (res != NULL && StrEquals(res, ""))
        {
            _Free(res);
            res = NULL;
        }

        tempStdio->Free();
        _Free(tempStdio);

        return res;
    }

};