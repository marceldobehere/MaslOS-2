#pragma once
#include <stdint.h>
#include <stddef.h>

#include <libm/rendering/virtualRenderer.h>
#include <libm/math.h>

extern uint32_t CurrentMouseCol;

void DrawMousePointer(MPoint point, PointerBuffer* framebuffer);
