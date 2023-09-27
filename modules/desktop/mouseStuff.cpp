#include "mouseStuff.h"
#include <libm/rendering/Cols.h>

uint32_t CurrentMouseCol = Colors.orange;

void DrawMousePointer(MPoint point, PointerBuffer* framebuffer)
{
    // for now we will just draw a 16x16 square for the mouse
    
    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 16; x++)
            if ((point.y + y) >= 0 && (point.y + y) < framebuffer->Height &&
                (point.x + x) >= 0 && (point.x + x) < framebuffer->Width)
                framebuffer->BaseAddress[(point.y + y) * framebuffer->Width + (point.x + x)] = &CurrentMouseCol;
        
}