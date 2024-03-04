
#include "BasicRenderer.h"
#include <libm/cstr.h>
#include <stdint.h>
#include <libm/rendering/Cols.h>
//#include "../cStdLib/cstrTools.h"
//#include "../memory/heap.h"

BasicRenderer *GlobalRenderer;

void BasicRenderer::putChar(char chr, int64_t xoff, int64_t yoff, uint32_t fg, uint32_t bg)
{
    delChar(xoff, yoff, bg);
    uint32_t tcol = color;
    bool toverwrite = overwrite;
    color = fg;
    overwrite = false;
    putChar(chr, xoff, yoff);
    overwrite = toverwrite;
    color = tcol;
}

void BasicRenderer::putChar(char chr, int64_t xoff, int64_t yoff)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;
    char *fontPtr = ((char *)psf1_font->glyphBuffer) + (chr * psf1_font->psf1_Header->charsize);

    if (overwrite)
        BasicRenderer::delChar(xoff, yoff);

    for (int64_t y = yoff; y < yoff + 16; y++)
    {
        for (int64_t x = xoff; x < xoff + 8; x++)
        {
            if (x >= 0 && x < framebuffer->Width && y >= 0 && y < framebuffer->Height)
                if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0)
                {
                    *(unsigned int *)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
                }
        }
        fontPtr++;
    }
}

void BasicRenderer::delChar(int64_t xoff, int64_t yoff, uint32_t col)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;

    for (int64_t y = yoff; y < yoff + 16; y++)
        for (int64_t x = xoff; x < xoff + 8; x++)
            if (x >= 0 && x < framebuffer->Width && y >= 0 && y < framebuffer->Height)
                *(uint32_t*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = col;
}

void BasicRenderer::delChar(int64_t xoff, int64_t yoff)
{
    BasicRenderer::delChar(xoff, yoff, 0x00000000);
}

void BasicRenderer::putStr(const char *chrs, int64_t xoff, int64_t yoff)
{
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
}

void BasicRenderer::putStr(const char *chrs, int64_t xoff, int64_t yoff, uint32_t col)
{
    uint32_t tcol = color;
    color = col;
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
    color = tcol;
}

void BasicRenderer::printStr(const char *chrs)
{
    printStr(chrs, NULL, true);
}

void BasicRenderer::printStr(const char *chrs, bool allowEscape)
{
    printStr(chrs, NULL, allowEscape);
}

void BasicRenderer::printStr(const char *chrs, const char *var)
{
    printStr(chrs, var, true);
}

void BasicRenderer::printStr(const char *chrs, const char *var, bool allowEscape)
{
    unsigned int index = 0;
    while (chrs[index] != 0)
    {
        if (CursorPosition.x >= framebuffer->Width)
        {
            CursorPosition.x = 0;
            CursorPosition.y += 16;
        }

        if (chrs[index] == '\n')
        {
            CursorPosition.y += 16;
        }
        else if (chrs[index] == '\r')
        {
            CursorPosition.x = 0;
        }
        else if (chrs[index] == '{' && allowEscape && var != NULL)
        {
            if (chrs[index + 1] == '}')
            {
                printStr(var);
                index++;
            }
        }
        else if (chrs[index] == '\\' && allowEscape)
        {
            if (chrs[index + 1] == '\\')
            {
                index++;
                putChar(chrs[index], CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == '%')
            {
                index++;
                putChar('%', CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == '{')
            {
                index++;
                putChar('{', CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == '}')
            {
                index++;
                putChar('}', CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
            else if (chrs[index + 1] == 'C')
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    putChar('?', CursorPosition.x, CursorPosition.y);
                }
                else
                {
                    index++;
                    color = ConvertStringToHex(&chrs[index]);
                    index += 5;
                }
            }
            else
            {
                putChar(chrs[index], CursorPosition.x, CursorPosition.y);
                CursorPosition.x += 8;
            }
        }
        else
        {
            putChar(chrs[index], CursorPosition.x, CursorPosition.y);
            CursorPosition.x += 8;
        }

        index++;
    }
}

void BasicRenderer::Println()
{
    BasicRenderer::printStr("\n\r");
}

void BasicRenderer::Print(char chr)
{
    char temp[] = {chr, 0};

    BasicRenderer::Print((const char *)temp, false);
}

void BasicRenderer::Print(const char *chrs)
{
    BasicRenderer::printStr(chrs);
}

void BasicRenderer::Print(const char *chrs, bool allowEscape)
{
    BasicRenderer::printStr(chrs, allowEscape);
}

void BasicRenderer::Println(const char *chrs)
{
    BasicRenderer::printStr(chrs);
    BasicRenderer::printStr("\n\r");
}

void BasicRenderer::Print(const char *chrs, const char *var)
{
    BasicRenderer::printStr(chrs, var);
}

void BasicRenderer::Println(const char *chrs, const char *var)
{
    BasicRenderer::printStr(chrs, var);

    BasicRenderer::printStr("\n\r");
}

void BasicRenderer::Print(const char *chrs, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    BasicRenderer::printStr(chrs);

    color = tempcol;
}

void BasicRenderer::Println(const char *chrs, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    BasicRenderer::printStr(chrs);
    BasicRenderer::printStr("\n\r");

    color = tempcol;
}

void BasicRenderer::Print(const char *chrs, const char *var, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    BasicRenderer::printStr(chrs, var);

    color = tempcol;
}

void BasicRenderer::Println(const char *chrs, const char *var, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    BasicRenderer::printStr(chrs, var);
    BasicRenderer::printStr("\n\r");

    color = tempcol;
}

void BasicRenderer::Clear(uint32_t col, bool resetCursor)
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;
    uint64_t fbHeight = framebuffer->Height;

    for (int64_t y = 0; y < framebuffer->Height; y++)
        for (int64_t x = 0; x < framebuffer->Width; x++)
            *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y))) = col;

    if (resetCursor)
        CursorPosition = {0, 0};
}

void BasicRenderer::ClearButDont()
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;
    uint64_t fbHeight = framebuffer->Height;

    for (int64_t y = 0; y < framebuffer->Height; y++)
        for (int64_t x = 0; x < framebuffer->Width; x++)
            *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y))) = *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y)));
}

void BasicRenderer::Clear(int64_t x1, int64_t y1, int64_t x2, int64_t y2, uint32_t col)
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;

    if (y1 < 0)
        y1 = 0;
    if (x1 < 0)
        x1 = 0;
    if (y2 >= framebuffer->Height)
        y2 = framebuffer->Height - 1;
    if (x2 >= framebuffer->Width)
        x2 = framebuffer->Width - 1;
    

    for (int64_t y = y1; y <= y2; y++)
        for (int64_t x = x1; x <= x2; x++)
            *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y))) = col;
}

void BasicRenderer::Clear(uint32_t col)
{
    BasicRenderer::Clear(col, true);
}

void BasicRenderer::ClearDotted(uint32_t col, bool resetCursor)
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;
    uint64_t fbHeight = framebuffer->Height;

    for (int64_t y = 0; y < framebuffer->Height; y += 2)
        for (int64_t x = 0; x < framebuffer->Width; x++)
            *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y))) = col;

    if (resetCursor)
        CursorPosition = {0, 0};
}

void BasicRenderer::ClearDotted(uint32_t col)
{
    ClearDotted(col, true);
}

void BasicRenderer::Cls()
{
    BasicRenderer::Clear(0);
    BasicRenderer::Println("(OLD) Masl OS 2 v0.01", Colors.green);
    BasicRenderer::Println("-------------------", Colors.green);
    BasicRenderer::Println();
}
 

void GlobDrawImage(kernelFiles::ImageFile* image, int64_t x, int64_t y, int64_t sx, int64_t sy, Framebuffer* framebuffer)
{
    x += image->xOff * sx;
    y += image->yOff * sy;

    uint64_t addr = (uint64_t)framebuffer->BaseAddress;
    uint64_t mult = framebuffer->PixelsPerScanLine;
    uint32_t* imgaddr = (uint32_t*)image->imageBuffer;
    for (int64_t y1 = 0; y1 < image->height; y1++)
    {
        for (int64_t x1 = 0; x1 < image->width; x1++)
        {
            if (*imgaddr != 0)//((*imgaddr/* | 0xffffff00*/) & (uint32_t)0xff000000 != (uint32_t)0x00000000)
            {
                for (int iy = 0; iy < sy; iy++)
                {
                    int64_t yp = (y1*sy) + iy + y;
                    for (int ix = 0; ix < sx; ix++)
                    {
                        int64_t xp = (x1*sx) + x + ix;
                        if (xp >= 0 && yp >= 0 && xp < framebuffer->Width && yp < framebuffer->Height)
                            *((uint32_t*)(addr + 4* (xp + mult * yp))) = *imgaddr;
                    }
                }
            }
            
            imgaddr ++;
        }
    }
}



void BasicRenderer::DrawImage(kernelFiles::ImageFile* image, int64_t x, int64_t y, int64_t sx, int64_t sy)
{
    GlobDrawImage(image, x, y, sx, sy, (Framebuffer*)framebuffer);
}

BasicRenderer::BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font)
{
    color = 0xffffffff;
    CursorPosition = {0, 0};
    this->framebuffer = framebuffer;
    this->psf1_font = psf1_font;
}

#include <libm/cstr.h>
#include <libm/cstrTools.h>

// %s -> string
// %c -> char
// %d/i -> int (32 bit)
// %D/I -> int (64 bit)
// %x -> hex (32 bit)
// %X -> hex (64 bit)
// %b -> byte
// %B -> bool
// %f -> float
// %F -> double
// %% -> %

void BasicRenderer::_Printf(const char* str, va_list arg, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    int len = StrLen(str);

    for (int i = 0; i < len; i++)
    {
        if (str[i] == '%' && i + 1 < len)
        {
            i++;
            if (str[i] == 's')
            {
                char* argStr = va_arg(arg, char*);
                if (argStr != NULL)
                    Print(argStr);
                else
                    Print("(null)");
            }
            else if (str[i] == 'c')
            {
                char argChar = va_arg(arg, int);
                Print(argChar);
            }
            else if (str[i] == 'd' || str[i + 1] == 'i')
            {
                int argInt = va_arg(arg, int);
                Print(to_string(argInt));
            }
            else if (str[i] == 'D' || str[i + 1] == 'I')
            {
                uint64_t argInt = va_arg(arg, uint64_t);
                Print(to_string(argInt));
            }
            else if (str[i] == 'x')
            {
                uint32_t argInt = va_arg(arg, uint32_t);
                Print(ConvertHexToString(argInt));
            }
            else if (str[i] == 'X')
            {
                uint64_t argInt = va_arg(arg, uint64_t);
                Print(ConvertHexToString(argInt));
            }
            else if (str[i] == 'b')
            {
                uint8_t argInt = (uint8_t)va_arg(arg, int);
                Print(to_string(argInt));
            }
            else if (str[i] == 'B')
            {
                bool argInt = (bool)va_arg(arg, int);
                Print(to_string(argInt));
            }
            else if (str[i] == 'f')
            {
                // compiler be trolling me
                // float argFloat = va_arg(arg, float);
                // Write(to_string(argFloat));
                

                double argDouble = va_arg(arg, double);
                Print(to_string(argDouble));
            }
            else if (str[i] == 'F')
            {
                double argDouble = va_arg(arg, double);
                Print(to_string(argDouble));
            }
            else if (str[i] == '%')
            {
                Print('%');
            }
            else
            {
                Print(str[i]);
            }
        }
        else
        {
            Print(str[i]);
        }
    }

    color = tempcol;    
}

void BasicRenderer::_Printf(const char* msg, va_list arg)
{
    _Printf(msg, arg, color);
}

void BasicRenderer::Printf(const char* msg, ...)
{
    va_list arg;
    va_start(arg, msg);
    _Printf(msg, arg);
    va_end(arg);
}

void BasicRenderer::Printfln(const char* msg, ...)
{
    va_list arg;
    va_start(arg, msg);
    _Printf(msg, arg);
    Println();
    va_end(arg);
}

void BasicRenderer::Printf(const char* msg, uint32_t col, ...)
{
    va_list arg;
    va_start(arg, col);
    _Printf(msg, arg, col);
    va_end(arg);
}

void BasicRenderer::Printfln(const char* msg, uint32_t col, ...)
{
    va_list arg;
    va_start(arg, col);
    _Printf(msg, arg, col);
    Println();
    va_end(arg);
}