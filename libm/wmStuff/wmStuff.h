#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../window/window.h"
#include "../msgPackets/msgPacket.h"
#include "../list/list_basic_msg.h"

void initWindowManagerStuff();
extern uint64_t desktopPID;

Window* getPartialWindow(uint64_t id);
void updateWindow(Window* window);
void setWindow(Window* window);
void setWindow(uint64_t id, Window* window);
Window* requestWindow(uint64_t id);
Window* requestWindow();
void deleteWindow(Window* window);
void deleteWindow(uint64_t id);

bool SendWindowFrameBufferUpdate(Window* window, int x1, int y1, int x2, int y2);
bool SendWindowFrameBufferUpdate(Window* window);

bool CheckForWindowClosed(Window* window);