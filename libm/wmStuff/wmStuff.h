#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../window/window.h"
#include "../msgPackets/msgPacket.h"
#include "../list/list_basic_msg.h"

void initWindowManagerStuff();
extern List<GenericMessagePacket*>* nonWindowPackets;
extern List<GenericMessagePacket*>* yesWindowPackets;
extern uint64_t desktopPID;

Window* getPartialWindow(uint64_t id);
void updateWindow(Window* window);
void setWindow(Window* window);
void setWindow(uint64_t id, Window* window);
Window* requestWindow();
void handleWindowPackets();

