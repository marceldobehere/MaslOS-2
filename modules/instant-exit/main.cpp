#include <libm/syscallManager.h>

int main()
{
    return 0;
    programSetPriority(1);
    while (true)
    {
        if (serialCanReadChar())
        {
            char c = serialReadChar();
            globalPrint("KEY> ");
            globalPrintChar(c);
            globalPrintLn("");

            if (c == '1')
                programYield();
            else if (c == '2')
                programWait(100);
            else if (c == '3')
                programSetPriority(0);
            else if (c == '4')
                programSetPriority(1);
            else if (c == '5')
                programSetPriority(2);
            else if (c == '6')
                programSetPriority(3);
            else if (c == '7')
                programSetPriority(20);
            else if (c == '8')
                programWait(1000);
        }
    }

    return 0;
}
