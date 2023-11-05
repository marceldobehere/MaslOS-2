#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>

char buffer[512];

int main(int argc, char** argv)
{
    programWait(1000);
    
    globalPrintLn("> Hello from FS Test");
    globalPrintLn("");

    bool testExists = false;
    bool bruhExists = false;

    {
        uint64_t driveCount = 0;
        const char** driveNames = fsGetDrivesInRoot(&driveCount);

        globalPrint("Found ");
        globalPrint(to_string(driveCount));
        globalPrintLn(" drives in root:");

        for (int i = 0; i < driveCount; i++)
        {
            globalPrint(" - ");
            globalPrintLn(driveNames[i]);
            if (StrEquals(driveNames[i], "test"))
                testExists = true;
            if (StrEquals(driveNames[i], "bruh"))
                bruhExists = true;

            _Free((void*)driveNames[i]);
        }
        _Free((void*)driveNames);
        globalPrintLn("");
    }

    

    globalPrint("Test drive exists: ");
    globalPrintLn(to_string(testExists));

    if (testExists)
    {
        {
            uint64_t folderCount = 0;
            const char** folderNames = fsGetFoldersInPath("test:", &folderCount);

            globalPrint("Found ");
            globalPrint(to_string(folderCount));
            globalPrintLn(" folders in test:");

            for (int i = 0; i < folderCount; i++)
            {
                globalPrint(" - ");
                globalPrintLn(folderNames[i]);

                _Free((void*)folderNames[i]);
            }
            if (folderNames != NULL)
                _Free((void*)folderNames);
            globalPrintLn("");
        }
    }

    globalPrint("Bruh drive exists: ");
    globalPrintLn(to_string(bruhExists));

    if (bruhExists)
    {
        {
            uint64_t folderCount = 0;
            const char** folderNames = fsGetFoldersInPath("bruh:", &folderCount);

            globalPrint("Found ");
            globalPrint(to_string(folderCount));
            globalPrintLn(" folders in bruh:");

            for (int i = 0; i < folderCount; i++)
            {
                globalPrint(" - ");
                globalPrintLn(folderNames[i]);

                _Free((void*)folderNames[i]);
            }
            if (folderNames != NULL)
                _Free((void*)folderNames);
            globalPrintLn("");
        }

        globalPrintLn("Creating Test Folder");
        bool creationWorked = fsCreateFolder("bruh:testo123");

        globalPrint("Creation worked: ");
        globalPrintLn(to_string(creationWorked));
        globalPrintLn("");

        {
           
            uint64_t folderCount = 0;

            const char** folderNames = fsGetFoldersInPath("bruh:", &folderCount);

            globalPrint("Found ");
            globalPrint(to_string(folderCount));
            globalPrintLn(" folders in bruh:");

            for (int i = 0; i < folderCount; i++)
            {
                globalPrint(" - ");
                globalPrintLn(folderNames[i]);

                _Free((void*)folderNames[i]);
            }
            if (folderNames != NULL)
                _Free((void*)folderNames);
            globalPrintLn("");
        }
    }

    
    return 0;
}
