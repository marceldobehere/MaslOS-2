#include <libm/syscallManager.h>
#include <libm/cstr.h>

void threadTest();

int val;

int main(int argc, char** argv)
{
    serialPrintLn("<MAIN START>");
    val = 123;

    serialPrint("VAL (1): ");
    serialPrintLn(to_string(val));

    uint64_t threadId = startThread((void*)threadTest);
    waitUntilThreadClosed(threadId);

    serialPrint("VAL (2): ");
    serialPrintLn(to_string(val));

    serialPrintLn("<MAIN END>");
    return 0;
}


void threadTest()
{
    serialPrintLn("<THREAD START>");
    programWait(500);

    serialPrintLn("Setting val to 456");
    val = 456;

    programWait(500);
    serialPrintLn("<THREAD END>");

    programExit(0); // Will just exit the thread
}