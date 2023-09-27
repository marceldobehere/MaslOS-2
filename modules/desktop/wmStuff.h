#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/window/window.h>


void DrawBGRect(int x1, int y1, int x2, int y2); //
void DrawTaskbarRect(int x1, int y1, int x2, int y2); //
void UpdatePointerRect(int x1, int y1, int x2, int y2); //
void UpdateWindowRect(Window *window); //
void RenderWindow(Window *window); // 
void ActuallyRenderWindow(Window *window); // 
void RenderWindowRect(Window *window, int x1, int y1, int x2, int y2); //
void RenderWindows(); //
void Clear(bool resetGlobal); //
void Render(); //
void UpdateWindowBorder(Window *window); //

uint64_t RenderActualSquare(int _x1, int _y1, int _x2, int _y2); //


void ClearFrameBuffer(Framebuffer* buffer, uint32_t col); //
void ClearPointerBuffer(PointerBuffer* buffer, uint32_t* col); //