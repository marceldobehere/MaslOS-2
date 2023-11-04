#pragma once
#include "lock.h"
#include <libm/queue/queue_mousePacket.h>

#ifdef _KERNEL_SRC

template class Lockable<Queue<MousePacket>>;
template class Lockable<Queue<MousePacket>*>;

#endif