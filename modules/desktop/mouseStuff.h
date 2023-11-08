#pragma once
#include <stdint.h>
#include <stddef.h>

#include <libm/rendering/virtualRenderer.h>
#include <libm/math.h>

extern uint32_t CurrentMouseCol;

void DrawMousePointer(MPoint point, PointerBuffer* framebuffer);

#include <libm/zips/basicZip.h>

extern ZipStuff::ZIPFile* mouseZIP;
extern ImageStuff::BitmapImage* currentMouseImage;
extern const char* currentMouseImageName;
extern const char* oldMouseImageName;

void FigureOutCorrectMouseImage();
void DrawMousePointerNew(MPoint point, PointerBuffer* framebuffer);
