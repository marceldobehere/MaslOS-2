#pragma once
#include <stdint.h>
#include <stddef.h>

#include <libm/rendering/virtualRenderer.h>
#include <libm/math.h>

#include <libm/window/window.h>
void MakeWinActive(Window* oldActive, Window* newActive);

extern uint32_t CurrentMouseCol;
extern bool DrawMouse;

void DrawMousePointer(MPoint point, PointerBuffer* framebuffer);

#include <libm/zips/basicZip.h>

extern ZipStuff::ZIPFile* mouseZIP;
extern ImageStuff::BitmapImage* currentMouseImage;
extern const char* currentMouseImageName;
extern const char* oldMouseImageName;

void FigureOutCorrectMouseImage();
void DrawMousePointerNew(MPoint point, PointerBuffer* framebuffer);

#include <libm/msgPackets/mousePacket/mousePacket.h>
bool HandleMouseClickPacket(MouseMessagePacket* packet);
bool HandleClick(bool L, bool R, bool M);
bool HandleDrag(bool L, bool R, bool M);
