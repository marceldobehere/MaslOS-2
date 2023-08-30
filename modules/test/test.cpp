#include "test.h"
#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>

int main()
{
    //return 0;
    //while (true);


    int argc = getArgC();
    //*((char*)(uint64_t)argc) = 'A';
    char** argv = getArgV();
    ENV_DATA* env = getEnvData();



    //return 0;
    
    main2(argc, argv, env);

    return 0;
}

void Bruhus(char* yes)
{
    for (int i = 0; i < 10; i++)
    {
        yes[i] = 0xFF;
    }
}

void main2(int argc, char** argv, ENV_DATA* env)
{
    globalCls();
    //return;
    
    globalPrintLn("Hello from a test module!");
    globalPrintLn("Yes, new line!");

    //return;
    TempRenderer renderer = TempRenderer(env->globalFrameBuffer, env->globalFont);
    renderer.Clear(Colors.bblue);
    
    //*((char*)(uint64_t)argc) = 'A';
    //Bruhus((char*)env->globalFrameBuffer->BaseAddress);
    
    //while (true);
}

