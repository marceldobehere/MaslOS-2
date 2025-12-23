#pragma once

#include <libm/list/list_window.h>
#include <libm/queue/queue_windowBufferUpdate.h>

extern Queue<WindowBufferUpdatePacket*>* updateFramePackets;
extern List<Window*>* windowsUpdated;

void InitEvents();
void HandleEvents();