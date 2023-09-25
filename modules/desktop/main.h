#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/syscallManager.h>
#include <libm/env/env.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/cstr.h>
#include <libm/list/list_window.h>
#include <libm/memStuff.h>




extern TempRenderer* actualScreenRenderer;
extern Framebuffer* actualScreenFramebuffer;
extern Framebuffer* mainBuffer;
extern PointerBuffer* pointerBuffer;

extern uint32_t defaultBackgroundColor;
extern Framebuffer* backgroundImage;
extern bool drawBackground;

extern Framebuffer* taskbar;

extern List<Window*>* windows;
extern Window* activeWindow;
extern Window* currentActionWindow;

extern MPoint MousePosition;

int main();

void DrawFrame();


void DrawBGRect(int x1, int y1, int x2, int y2); //
void DrawTaskbarRect(int x1, int y1, int x2, int y2); //
void UpdatePointerRect(int x1, int y1, int x2, int y2); //
void UpdateWindowRect(Window *window); //
void RenderWindow(Window *window); // 
void RenderWindowRect(Window *window, int x1, int y1, int x2, int y2); //
void RenderWindows(); //
void Clear(bool resetGlobal); //
void Render(); //
void UpdateWindowBorder(Window *window); //

uint64_t RenderActualSquare(int _x1, int _y1, int _x2, int _y2);


void ClearFrameBuffer(Framebuffer* buffer, uint32_t col); //
void ClearPointerBuffer(PointerBuffer* buffer, uint32_t* col); //



/*
    programSetPriority(1);
    while (true)
    {
        if (serialCanReadChar())
        {
            char c = serialReadChar();
            globalPrint("KEY> ");
            globalPrintChar(c);
            globalPrintLn("");

            if (c == '1')
                programYield();
            else if (c == '2')
                programWait(100);
            else if (c == '3')
                programSetPriority(0);
            else if (c == '4')
                programSetPriority(1);
            else if (c == '5')
                programSetPriority(2);
            else if (c == '6')
                programSetPriority(3);
            else if (c == '7')
                programSetPriority(20);
            else if (c == '8')
                programWait(1000);
        }
    }
*/