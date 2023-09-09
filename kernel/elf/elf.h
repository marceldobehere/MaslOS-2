#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/elf/elfDefines.h>
#include <libm/env/env.h>


namespace Elf
{
    extern LoadedElfFile borkedElfFile;

    LoadedElfFile LoadElf(uint8_t* data);

	void FreeElf(LoadedElfFile file);

    //void RunElfHere(LoadedElfFile file, int argc, char** argv, ENV_DATA* envData);

}