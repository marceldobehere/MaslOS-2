#pragma once
#include "./queue.h"

#include <libm/msgPackets/windowBufferUpdatePacket/windowBufferUpdatePacket.h>

template class Queue<WindowBufferUpdatePacket*>;
