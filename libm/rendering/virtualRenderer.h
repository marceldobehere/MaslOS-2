#pragma once
#include <stdint.h>
#include <stddef.h>
#include "S-Font.h"
#include "framebuffer.h"
#include <libm/images/bitmapImage.h>

namespace VirtualRenderer
{
    extern PSF1_FONT* psf1_font;

    struct Border
    {
        int x1, y1, x2, y2;

        Border(int x1, int y1, int x2, int y2)
        {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }

        Border(PointerBuffer* framebuffer)
        {
            this->x1 = 0;
            this->y1 = 0;
            this->x2 = framebuffer->Width;
            this->y2 = framebuffer->Height;
        }  
    };


    void DrawImage(ImageStuff::BitmapImage* image, int64_t x, int64_t y, int64_t sx, int64_t sy, Border border, PointerBuffer* framebuffer);

    void Clear(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Border border, PointerBuffer* framebuffer, uint32_t* col);

    void putStr(const char *chrs, int64_t xoff, int64_t yoff, Border border, PointerBuffer* framebuffer, uint32_t* col);

    void putChar(char chr, int64_t xoff, int64_t yoff, Border border, PointerBuffer* framebuffer, uint32_t* col);

    void DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, Border border, PointerBuffer* framebuffer, uint32_t* col);
}