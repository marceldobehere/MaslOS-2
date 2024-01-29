#include <libm/syscallManager.h>
#include <libm/cstr.h>
#include <libm/memStuff.h>

void threadTest1();
void threadTest2();

int main(int argc, char** argv)
{
    serialPrintLn("<MAIN START>");

    serialPrint("MALLOC COUNT (1): ");
    serialPrintLn(to_string(Heap::GlobalHeapManager->_usedHeapCount));

    uint64_t threadId1 = startThread((void*)threadTest1);
    uint64_t threadId2 = startThread((void*)threadTest2);
    waitUntilThreadClosed(threadId1);
    waitUntilThreadClosed(threadId2);

    serialPrint("MALLOC COUNT (2): ");
    serialPrintLn(to_string(Heap::GlobalHeapManager->_usedHeapCount));

    serialPrintLn("<MAIN END>");
    return 0;
}

const int MallocCount = 1000;
const int MallocSize = 345;
void threadTestGeneric(uint8_t tVal, int threadId);

void threadTest1()
{
    threadTestGeneric(0xA1, 1);
    programExit(0);
}
void threadTest2()
{
    threadTestGeneric(0xFB, 2);
    programExit(0);
}

void threadStartMsg(int threadId)
{
    serialPrint("<THREAD ");
    serialPrint(to_string(threadId));
    serialPrint(" ");
}
void threadStartMsg(int threadId, const char* msg)
{
    threadStartMsg(threadId);
    serialPrintLn(msg);
}

void threadTestGeneric(uint8_t tVal, int threadId)
{
    threadStartMsg(threadId, "START>");
    programWait(800);

    threadStartMsg(threadId, "MALLOC>");
    void* bruhus[MallocCount];
    for (int i = 0; i < MallocCount; i++)
        bruhus[i] = _Malloc(MallocSize);

    programWait(200);

    threadStartMsg(threadId, "FILL WITH DATA>");
    for (int i = 0; i < MallocCount; i++)
        _memset(bruhus[i], tVal, MallocSize);
    threadStartMsg(threadId, "FILLED WITH DATA>");

    programWait(2000);

    bool dataOk = true;
    threadStartMsg(threadId, "VERIFY DATA>");
    for (int i = 0; i < MallocCount; i++)
        for (int j = 0; j < MallocSize; j++)
            if (((uint8_t*)bruhus[i])[j] != tVal)
            {
                dataOk = false;
                break;
            }

    programWait(200);

    threadStartMsg(threadId);
    serialPrint("DATA OK: ");
    serialPrint(to_string(dataOk));
    serialPrintLn(">");

    programWait(500);

    threadStartMsg(threadId, "FREE>");
    for (int i = 0; i < MallocCount; i++)
        _Free(bruhus[i]);

    programWait(800);
    threadStartMsg(threadId, "END>");

    programExit(0); // Will just exit the thread
}