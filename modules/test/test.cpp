#include "test.h"

void _start(int argc, char** argv, ENV_DATA* env)
{
    //return;
    //while (true);

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
    Bruhus((char*)env->globalFrameBuffer->BaseAddress);
    //while (true);
}

