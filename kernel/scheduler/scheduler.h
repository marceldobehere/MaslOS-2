#pragma once
#include <stdint.h>
#include <stddef.h>

#include "../interrupts/interrupts.h"

struct osTask
{

};

void initScheduler();

// maybe save more registers yes
void scheduleInterrupt(interrupt_frame* frame);

