#pragma once
#include "./queue.h"

#ifdef _KERNEL_SRC

#include "../../kernel/devices/mouse/mousePacket.h"

template class Queue<MousePacket>;

#endif