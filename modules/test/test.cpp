#include "test.h"
#include <libm/syscallManager.h>

void _start()
{
    //*((char*)(uint64_t)argc) = 'A';
    //return;
    //while (true);

    

    int argc = getArgC();
    //*((char*)(uint64_t)argc) = 'A';
    char** argv = getArgV();
    ENV_DATA* env = getEnvData();

    //return;
    
    main(argc, argv, env);
}

void Bruhus(char* yes)
{
    for (int i = 0; i < 10; i++)
    {
        yes[i] = 0xFF;
    }
}

void main(int argc, char** argv, ENV_DATA* env)
{
    //*((char*)(uint64_t)argc) = 'A';
    Bruhus((char*)env->globalFrameBuffer->BaseAddress);
    //while (true);
}

