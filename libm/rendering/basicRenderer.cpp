
#include "basicRenderer.h"
#include <libm/cstr.h>
#include <stdint.h>
#include <libm/rendering/Cols.h>
//#include "../cStdLib/cstrTools.h"
//#include "../memory/heap.h"


void TempRenderer::putChar(char chr, int64_t xoff, int64_t yoff, uint32_t fg, uint32_t bg)
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

void TempRenderer::putChar(char chr, int64_t xoff, int64_t yoff)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;
    char *fontPtr = ((char *)psf1_font->glyphBuffer) + (chr * psf1_font->psf1_Header->charsize);

    if (overwrite)
        TempRenderer::delChar(xoff, yoff);

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

void TempRenderer::delChar(int64_t xoff, int64_t yoff, uint32_t col)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;

    for (int64_t y = yoff; y < yoff + 16; y++)
        for (int64_t x = xoff; x < xoff + 8; x++)
            if (x >= 0 && x < framebuffer->Width && y >= 0 && y < framebuffer->Height)
                *(uint32_t*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = col;
}

void TempRenderer::delChar(int64_t xoff, int64_t yoff)
{
    TempRenderer::delChar(xoff, yoff, 0x00000000);
}

void TempRenderer::putStr(const char *chrs, int64_t xoff, int64_t yoff)
{
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
}

void TempRenderer::putStr(const char *chrs, int64_t xoff, int64_t yoff, uint32_t col)
{
    uint32_t tcol = color;
    color = col;
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
    color = tcol;
}

void TempRenderer::printStr(const char *chrs)
{
    printStr(chrs, NULL, true);
}

void TempRenderer::printStr(const char *chrs, bool allowEscape)
{
    printStr(chrs, NULL, allowEscape);
}

void TempRenderer::printStr(const char *chrs, const char *var)
{
    printStr(chrs, var, true);
}

void TempRenderer::printStr(const char *chrs, const char *var, bool allowEscape)
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

void TempRenderer::Println()
{
    TempRenderer::printStr("\n\r");
}

void TempRenderer::Print(char chr)
{
    char temp[] = {chr, 0};

    TempRenderer::Print((const char *)temp, false);
}

void TempRenderer::Print(const char *chrs)
{
    TempRenderer::printStr(chrs);
}

void TempRenderer::Print(const char *chrs, bool allowEscape)
{
    TempRenderer::printStr(chrs, allowEscape);
}

void TempRenderer::Println(const char *chrs)
{
    TempRenderer::printStr(chrs);
    TempRenderer::printStr("\n\r");
}

void TempRenderer::Print(const char *chrs, const char *var)
{
    TempRenderer::printStr(chrs, var);
}

void TempRenderer::Println(const char *chrs, const char *var)
{
    TempRenderer::printStr(chrs, var);

    TempRenderer::printStr("\n\r");
}

void TempRenderer::Print(const char *chrs, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    TempRenderer::printStr(chrs);

    color = tempcol;
}

void TempRenderer::Println(const char *chrs, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    TempRenderer::printStr(chrs);
    TempRenderer::printStr("\n\r");

    color = tempcol;
}

void TempRenderer::Print(const char *chrs, const char *var, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    TempRenderer::printStr(chrs, var);

    color = tempcol;
}

void TempRenderer::Println(const char *chrs, const char *var, uint32_t col)
{
    uint64_t tempcol = color;
    color = col;

    TempRenderer::printStr(chrs, var);
    TempRenderer::printStr("\n\r");

    color = tempcol;
}

void TempRenderer::Clear(uint32_t col, bool resetCursor)
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

void TempRenderer::ClearButDont()
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;
    uint64_t fbHeight = framebuffer->Height;

    for (int64_t y = 0; y < framebuffer->Height; y++)
        for (int64_t x = 0; x < framebuffer->Width; x++)
            *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y))) = *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y)));
}

void TempRenderer::Clear(int64_t x1, int64_t y1, int64_t x2, int64_t y2, uint32_t col)
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

void TempRenderer::Clear(uint32_t col)
{
    TempRenderer::Clear(col, true);
}

void TempRenderer::ClearDotted(uint32_t col, bool resetCursor)
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

void TempRenderer::ClearDotted(uint32_t col)
{
    ClearDotted(col, true);
}

void TempRenderer::Cls()
{
    TempRenderer::Clear(0);
    TempRenderer::Println("(OLD) Masl OS 2 v0.01", Colors.green);
    TempRenderer::Println("-------------------", Colors.green);
    TempRenderer::Println();
}

TempRenderer::TempRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font)
{
    color = 0xffffffff;
    CursorPosition = {0, 0};
    this->framebuffer = framebuffer;
    this->psf1_font = psf1_font;
}