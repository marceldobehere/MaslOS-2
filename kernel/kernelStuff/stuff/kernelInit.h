#pragma once
#include "../memory/memory.h"

void InitKernel(BootInfo* bootInfo);

void PrepareMemory(BootInfo* bootInfo);

void PrepareInterrupts();

void DoGdtStuff();