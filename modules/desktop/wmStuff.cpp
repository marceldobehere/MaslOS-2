#include "wmStuff.h"
#include "main.h"
#include <libm/rendering/virtualRenderer.h>
#include <libm/cstrTools.h>


void UpdatePointerRect(int x1, int y1, int x2, int y2)
{
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x1 >= pointerBuffer->Width)
        x1 = pointerBuffer->Width - 1;
    if (y1 >= pointerBuffer->Height)
        y1 = pointerBuffer->Height - 1;

    if (x2 < 0)
        x2 = 0;
    if (y2 < 0)
        y2 = 0;
    if (x2 >= pointerBuffer->Width)
        x2 = pointerBuffer->Width - 1;
    if (y2 >= pointerBuffer->Height)
        y2 = pointerBuffer->Height - 1;


    if (x1 > x2 || y1 > y2)
        return;


    DrawBGRect(x1, y1, x2, y2);
    

    int count = windows->GetCount();
    for (int i = 0; i < count; i++)
        RenderWindowRect(windows->ElementAt(i), x1, y1, x2, y2);

    DrawTaskbarRect(x1, y1, x2, y2);
}


void DrawBGRect(int x1, int y1, int x2, int y2)
{
    if (drawBackground)
    {
        for (int y = y1; y <= y2; y++)
        {
            int64_t yIndex = (((y * backgroundImage->Height)/pointerBuffer->Height)*backgroundImage->Width);
            for (int x = x1; x <= x2; x++)
            {
                int64_t index = x + y * pointerBuffer->Width;
                int64_t index2 = ((x * backgroundImage->Width)/pointerBuffer->Width) + yIndex;
                (((uint32_t**)pointerBuffer->BaseAddress)[index]) = &((uint32_t*)backgroundImage->BaseAddress)[index2];
            }
        }
    }
    else
    {
        uint32_t* colPointer = &defaultBackgroundColor;
        for (int y = y1; y <= y2; y++)
        {
            int64_t yIndex = y * pointerBuffer->Width;
            for (int x = x1; x <= x2; x++)
            {    
                (((uint32_t**)pointerBuffer->BaseAddress)[x + yIndex]) = colPointer;
            }   
        } 
    }
}


void RenderWindowRect(Window* window, int x1, int y1, int x2, int y2)
{
    if (window == NULL)
        return;

    if (window->Hidden)
        return;
    

    // if (window == osData.debugTerminalWindow && !osData.showDebugterminal)
    //     return;
    

    int64_t _x1 = window->Dimensions.x;
    int64_t _y1 = window->Dimensions.y;
    int64_t _x2 = _x1 + window->Dimensions.width - 1;
    int64_t _y2 = _y1 + window->Dimensions.height - 1;

    if ((_x1 - 1) > x2 || (_x2 + 1) < x1 || (_y1 - 23) > y2 || (_y2 + 1) < y1)
        return;

    { 
        if (y1 > _y1)
            _y1 = y1;

        if (y2 < _y2)
            _y2 = y2;

        if (x1 > _x1)
            _x1 = x1;

        if (x2 < _x2)
            _x2 = x2;

        if (0 > _y1)
            _y1 = 0;

        if (pointerBuffer->Height <= _y2)
            _y2 = pointerBuffer->Height - 1;

        if (0 > _x1)
            _x1 = 0;

        if (pointerBuffer->Width <= _x2)
            _x2 = pointerBuffer->Width - 1;


        for (int64_t y = _y1; y <= _y2; y++)
        {
            for (int64_t x = _x1; x <= _x2; x++)
            {
                int64_t index = x + y * pointerBuffer->Width; 
                int64_t index2 = (x - window->Dimensions.x) + (y - window->Dimensions.y) * window->Buffer->Width; 
                
                (((uint32_t**)pointerBuffer->BaseAddress)[index]) = &((uint32_t*)window->Buffer->BaseAddress)[index2];
            }
        }

    }
    //return;
    

    {
        _x1 = window->Dimensions.x - 1;
        _y1 = window->Dimensions.y - 23;
        _x2 = _x1 + window->Dimensions.width + 2;//3;
        _y2 = _y1 + window->Dimensions.height + 24;//25;
    
        if (y1 > _y1)
            _y1 = y1;

        if (y2 < _y2)
            _y2 = y2;

        if (x1 > _x1)
            _x1 = x1;

        if (x2 < _x2)
            _x2 = x2;

        if (0 > _y1)
            _y1 = 0;

        if (pointerBuffer->Height <= _y2)
            _y2 = pointerBuffer->Height - 1;

        if (0 > _x1)
            _x1 = 0;

        if (pointerBuffer->Width <= _x2)
            _x2 = pointerBuffer->Width - 1;
    }

    

    if (_x1 > _x2 || _y1 > _y2)
        return;
    
    if (window->ShowTitleBar)
    {
        int64_t x = window->Dimensions.x;
        int64_t y = window->Dimensions.y- 21;
        VirtualRenderer::Clear(x,y, x + window->Dimensions.width-1, window->Dimensions.y-2, VirtualRenderer::Border(_x1, _y1, _x2, _y2), pointerBuffer, &window->DefaultTitleBackgroundColor);

        const char* stitle = StrSubstr(window->Title, 0, (window->Dimensions.width - 60) / 8);


        // if (window->instance != NULL)
        // {
        //     if (window->instance->instanceType == InstanceType::Terminal)
        //     {
        //         TerminalInstance* terminal = (TerminalInstance*)window->instance;
        //         free((void*)stitle);
        //         stitle = StrCopy(to_string(terminal->tasks.getCount()));
        //     }
        // }

        if (activeWindow == window)
            VirtualRenderer::putStr(stitle, x, y, VirtualRenderer::Border(_x1, _y1, _x2, _y2), pointerBuffer, &window->SelectedTitleColor);
        else
            VirtualRenderer::putStr(stitle, x, y, VirtualRenderer::Border(_x1, _y1, _x2, _y2), pointerBuffer, &window->DefaultTitleColor);
        
        
        _Free((void*)stitle);
    }

    //return;
    
    if (window->ShowBorder)
    {
        uint32_t** arr = ((uint32_t**)pointerBuffer->BaseAddress);
        int64_t width = pointerBuffer->Width;

        uint32_t* cBorder = &window->DefaultBorderColor;
        if (activeWindow == window)
            cBorder = &window->SelectedBorderColor;

        uint8_t counter = 0;
        for (int64_t x = -1; x < window->Dimensions.width + 1; x++)
        {
            int64_t newX = x + window->Dimensions.x;
            int64_t newY = -1 + window->Dimensions.y;
            if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 1)
                arr[newX + newY * width] = cBorder; //*(uint32_t*)(to->BaseAddress + ((newX + (newY * to->Width)) * 4))
            
            
            newY = window->Dimensions.height + window->Dimensions.y;
            if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                arr[newX + newY * width] = cBorder;
            
            // if (window->showTitleBar)
            // {
            //     newY = -22 + window->position.y;
            //     if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
            //         arr[newX + newY * width] = cBorder;
            // }
        
            counter++;
        }

        if (window->ShowTitleBar)
        {
            counter = 0;
            for (int64_t x = -1; x < window->Dimensions.width + 1; x++)
            {
                int64_t newX = x + window->Dimensions.x;
                int64_t newY = -22 + window->Dimensions.y;
                    
                if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                    arr[newX + newY * width] = cBorder;
                
                counter++;
            }  
        }


        counter = 0;
        int64_t maxY = -22;
        if (!window->ShowTitleBar)
            maxY = -1;
        for (int64_t y = maxY; y < window->Dimensions.height; y++)
        {
            int64_t newX = window->Dimensions.width + window->Dimensions.x;
            int64_t newY = y + window->Dimensions.y;
            if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 1)
                arr[newX + newY * width] = cBorder;
            
            newX = -1 + window->Dimensions.x;
            if (newX >= _x1 && newY >= _y1 && newX <= _x2 && newY <= _y2 && (counter % 2) == 0)
                arr[newX + newY * width] = cBorder;
            
            counter++;
        } 
    }

    
    if (window->ShowTitleBar)
    {
        VirtualRenderer::Border border = VirtualRenderer::Border(_x1, _y1, _x2, _y2);
        int64_t x = window->Dimensions.x + window->Dimensions.width;
        int64_t y = window->Dimensions.y - 22;

        
        {
            int state = 0;
            if (activeWindow == window)
                state = 1;
            if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
                state = 2;
            if (state == 2)
                currentActionWindow = window;

            //VirtualRenderer::DrawImage(windowButtonIcons[windowButtonIconEnum.CLOSE_N + state], x - 20, y + 1, 1, 1, border, virtualScreenBuffer);
            x -= 20;
        }
        {
            int state = 0;
            if (activeWindow == window)
                state = 1;
            if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
                state = 2;
            if (state == 2)
                currentActionWindow = window;

            //VirtualRenderer::DrawImage(windowButtonIcons[windowButtonIconEnum.MIN_N + state], x - 20, y + 1, 1, 1, border, pointerBuffer);
            x -= 20;
        }
        {
            int state = 0;
            if (activeWindow == window)
                state = 1;
            if (MousePosition.x >= x - 20 && MousePosition.x <= x && MousePosition.y >= y && MousePosition.y <= y + 20)
                state = 2;
            if (state == 2)
                currentActionWindow = window;

            //VirtualRenderer::DrawImage(windowButtonIcons[windowButtonIconEnum.HIDE_N + state], x - 20, y + 1, 1, 1, border, pointerBuffer);
            x -= 20;
        }

        
    }

    {
        //uint32_t col = Colors.bred;
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 5, _x1 + 20, _y1 + 10, border, virtualScreenBuffer,  &window->defaultBorderColor);
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 10, _x1 + 1, _y1 + 11, border, virtualScreenBuffer, &window->selectedBorderColor);
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 14, _x1 + 8, _y1 + 17, border, virtualScreenBuffer, &window->selectedBorderColor);
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 18, _x1 + 6, _y1 + 25, border, virtualScreenBuffer, &window->defaultBorderColor);
        

        //windowIcons[windowIconEnum.CLOSE_N]

        //VirtualRenderer::DrawImage(kernelFiles::ConvertFileToImage(&osData.windowIconsZIP->files[0]), x - 30, y + 10, 1, 1, border, virtualScreenBuffer);
        //VirtualRenderer::DrawImage(kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(osData.windowIconsZIP, WindowManager::windowIconNames[3])), x - 60, y + 40, 1, 1, border, virtualScreenBuffer);
        //osData.debugTerminalWindow->Log("- ADDR A: {}", ConvertHexToString((uint64_t)windowIcons[0]), Colors.yellow);
        //VirtualRenderer::DrawImage(windowIcons[windowIconEnum.MIN_H], x - 90, y + 50, 2, 2, border, virtualScreenBuffer);

        //VirtualRenderer::Border border = VirtualRenderer::Border(_x1, _y1, _x2, _y2);
        //int64_t x = window->position.x + window->size.width;
        //uint32_t col = Colors.bred;
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 5, _x1 + 20, _y1 + 10, border, virtualScreenBuffer,  &window->defaultBorderColor);
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 10, _x1 + 1, _y1 + 11, border, virtualScreenBuffer, &window->selectedBorderColor);
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 14, _x1 + 8, _y1 + 17, border, virtualScreenBuffer, &window->selectedBorderColor);
        //VirtualRenderer::DrawLine(_x1 + 5, _y1 + 18, _x1 + 6, _y1 + 25, border, virtualScreenBuffer, &window->defaultBorderColor);
        

    }




} 



void DrawTaskbarRect(int x1, int y1, int x2, int y2)
{
    int64_t ypos = pointerBuffer->Height - taskbar->Height;

    if (y2 < ypos)
        return;
    if (y1 < ypos)
        y1 = ypos;

    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
        {
            int64_t index = x + y * pointerBuffer->Width;
            int64_t index2 = x + (y-ypos) * pointerBuffer->Width;
            (((uint32_t**)pointerBuffer->BaseAddress)[index]) = &((uint32_t*)taskbar->BaseAddress)[index2];//&defaultBackgroundColor;
        }
}

void UpdateWindowRect(Window* window)
{
    UpdatePointerRect(window->Dimensions.x - 1, window->Dimensions.y - 24, window->Dimensions.x + window->Dimensions.width + 1, window->Dimensions.y + window->Dimensions.height + 1);
}

void ActuallyRenderWindow(Window *window)
{
    int x1 = max(0, window->Dimensions.x - 1);
    int y1 = max(0, window->Dimensions.y - 24);
    int x2 = min(pointerBuffer->Width - 1, window->Dimensions.x + window->Dimensions.width + 1);
    int y2 = min(pointerBuffer->Height- 1, window->Dimensions.y + window->Dimensions.height + 1);

    RenderWindowRect(window, 
        x1, y1, 
        x2, y2
    );
    
    DrawTaskbarRect(x1, y1, x2, y2);

    RenderActualSquare(
        x1, y1, 
        x2, y2
    );
}

void RenderWindow(Window* window)
{
    int x1 = max(0, window->Dimensions.x - 1);
    int y1 = max(0, window->Dimensions.y - 24);
    int x2 = min(pointerBuffer->Width - 1, window->Dimensions.x + window->Dimensions.width + 1);
    int y2 = min(pointerBuffer->Height- 1, window->Dimensions.y + window->Dimensions.height + 1);

    RenderWindowRect(window, 
        x1, y1, 
        x2, y2
    );
    
    DrawTaskbarRect(x1, y1, x2, y2);
}

void RenderWindows()
{
    UpdatePointerRect(0, 0, pointerBuffer->Width - 1, pointerBuffer->Height - 1);
} 



void Clear(bool resetGlobal)
{
    if (resetGlobal)
        ClearFrameBuffer(actualScreenFramebuffer, defaultBackgroundColor);
    ClearFrameBuffer(mainBuffer, defaultBackgroundColor);
    ClearFrameBuffer(taskbar, Colors.dblue);

    ClearPointerBuffer(pointerBuffer, &defaultBackgroundColor);
    //ClearPointerBuffer(copyOfVirtualBuffer, &defaultBackgroundColor);
}   


void ClearFrameBuffer(Framebuffer* buffer, uint32_t col)
{
    for (uint32_t y = 0; y < buffer->Height; y++)
        for (uint32_t x = 0; x < buffer->Width; x++)
            *((uint32_t*)buffer->BaseAddress + x + (y * buffer->PixelsPerScanLine)) = col;
}

void ClearPointerBuffer(PointerBuffer* buffer, uint32_t* col)
{
    uint32_t** endAddr = (uint32_t**)((uint64_t)buffer->BaseAddress + buffer->BufferSize);
    for (uint32_t** pixel = (uint32_t**)buffer->BaseAddress; pixel < endAddr; pixel++)
        *pixel = col;
}


uint8_t testInterlace = 1;
uint8_t testCounterX = 0;
uint8_t testCounterY = 0;

void Render()
{
    // if (osData.currentDisplay == NULL)
    //     return;
    
    
    uint64_t counta = 0;
    
    // if (actualScreenBuffer != osData.currentDisplay->framebuffer)
    // {
    //     int sizeX = osData.currentDisplay->framebuffer->Width;
    //     int sizeY = osData.currentDisplay->framebuffer->Height;
    //     int sizePPS = osData.currentDisplay->framebuffer->PixelsPerScanLine;

    //     if (actualScreenBuffer->Width != sizeX || 
    //         actualScreenBuffer->Height != sizeY || 
    //         actualScreenBuffer->PixelsPerScanLine != sizePPS)
    //     {
    //         // RESIZE
    //         Resize(osData.currentDisplay->framebuffer);
    //         Clear(true);
    //         RenderWindows();
    //     }
    //     else
    //         actualScreenBuffer = osData.currentDisplay->framebuffer;
    // }

    // osData.currentDisplay->StartFrame();

    if (testInterlace != 1 && testInterlace != 0)
    {
        
        int64_t h = actualScreenFramebuffer->Height, w = actualScreenFramebuffer->Width;

        uint32_t** vPixel = (uint32_t**)pointerBuffer->BaseAddress + w * testCounterY;// + testCounterX;
        uint32_t*  cPixel = (uint32_t*)  mainBuffer->BaseAddress + w * testCounterY;// + testCounterX;

        uint8_t testInterlaceMinusOne = testInterlace - 1;
        uint64_t wTimesInterlaceMinusOne = w * testInterlaceMinusOne;

        // uint32_t** bVPixel = (uint32_t**)virtualScreenBuffer->BaseAddress;// + w * testCounterY;
        // uint32_t* bCPixel = (uint32_t*) copyOfScreenBuffer->BaseAddress;// + w * testCounterY;


        for (int64_t y = testCounterY; y < h; y += testInterlace)
        {
            for (int64_t x = testCounterX; x < w;)
            {
                uint32_t col = *(vPixel[x]);
                if (cPixel[x] != col)
                {
                    // BEFORE
                    // x -= testCounterX;
                    // y -= testCounterY;
                    // uint32_t** vPixel1 = vPixel;
                    // uint32_t*  cPixel1 = cPixel;
                    // vPixel -= testCounterX + (testCounterY * w);
                    // cPixel -= testCounterX + (testCounterY * w); 

                    // counta += RenderActualSquare(
                    //                                         max(x, 0), 
                    //                                         max(y, 0), 
                    //                                         min(x + testInterlace, w - 1), 
                    //                                         min(y + testInterlace, h - 1)
                    //                                     );

                    //
                    counta += RenderActualSquare(
                        x - (testInterlace * 2 - 1), 
                        y - (testInterlace * 2 - 1), 
                        
                        x + testInterlace * 4 - 2, 
                        y + testInterlace * 4 - 2
                        );
                    //

                    // vPixel += testInterlace * 2;
                    // cPixel += testInterlace * 2;
                    x       += testInterlace * 3;

                    // AFTER
                    // x += testCounterX;
                    // y += testCounterY;
                    // vPixel = vPixel1;
                    // cPixel = cPixel1;
                }
                // vPixel += testInterlace;
                // cPixel += testInterlace;
                x += testInterlace;
            } 
            // vPixel += wTimesInterlaceMinusOne;
            // cPixel += wTimesInterlaceMinusOne;
            vPixel += testInterlace * w;
            cPixel += testInterlace * w;
        }
        
    }
    else
    {
        if (testInterlace == 1)
        {
            
            uint64_t h = actualScreenFramebuffer->Height, w = actualScreenFramebuffer->Width;

            uint32_t** vPixel = (uint32_t**)pointerBuffer->BaseAddress;
            uint32_t*  cPixel = (uint32_t*)  mainBuffer->BaseAddress;
            uint32_t*  aPixel = (uint32_t*)  actualScreenFramebuffer->BaseAddress;

            
            for (int64_t y = 0; y < h; y++)
            {
                aPixel = (uint32_t*)  actualScreenFramebuffer->BaseAddress + y * actualScreenFramebuffer->PixelsPerScanLine;
                for (int64_t x = 0; x < w; x++)
                {
                    uint32_t col = **vPixel;
                    if (*cPixel != col)
                    {
                        *aPixel = col;
                        *cPixel = col;
                        //osData.currentDisplay->UpdatePixel(x, y);
                    }
                    vPixel++;
                    cPixel++;
                    aPixel++;
                } 
            }
            

            
        }
        else
        {
            
            uint64_t h = actualScreenFramebuffer->Height, w = actualScreenFramebuffer->Width;

            uint32_t** vPixel = (uint32_t**)pointerBuffer->BaseAddress;
            uint32_t*  aPixel = (uint32_t*)  actualScreenFramebuffer->BaseAddress;

            
            for (int64_t y = 0; y < h; y++)
            {
                aPixel = (uint32_t*)  actualScreenFramebuffer->BaseAddress + y * actualScreenFramebuffer->PixelsPerScanLine;
                for (int64_t x = 0; x < w; x++)
                {
                    *aPixel = **vPixel;
                    vPixel++;
                    aPixel++;
                    //osData.currentDisplay->UpdatePixel(x, y);
                } 
            }
            

            
        }
    }

    //osData.currentDisplay->EndFrame();

    
    // //osData.debugTerminalWindow->Log("             : ################", Colors.black);
    // osData.debugTerminalWindow->renderer->CursorPosition.x = 0;
    // osData.debugTerminalWindow->renderer->CursorPosition.y -= 16 * 16;

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Pixel changed: {}", to_string(counta), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("FPS: {}", to_string(fps), Colors.yellow);
    


    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 200,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Used Heap count: {}", to_string(usedHeapCount), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 300,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Used Heap amount: {} KB", to_string(usedHeapAmount / 0x1000), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Mouse Packet Count: {}", to_string(mousePackets.GetCount()), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Malloc Count: {}", to_string(mallocCount), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Free Count: {}", to_string(freeCount), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Stack Trace Count: {}", to_string(MStackData::stackPointer+1), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("Last Free Size: {}", to_string(lastFreeSize), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("PORT 64 VAL: {}", ConvertHexToString(osData.port64Val), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    
    // uint64_t tVal = 0;

    // if (MStackData::BenchmarkMode == 0)
    //     tVal = MStackData::BenchmarkStackPointer2;//osData.debugTerminalWindow->Log("MSTACK: {}", to_string(MStackData::BenchmarkStackPointer2), Colors.yellow);
    // else
    //     tVal = MStackData::BenchmarkStackPointer1;//osData.debugTerminalWindow->Log("MSTACK: {}", to_string(MStackData::BenchmarkStackPointer1), Colors.yellow);
    // if (tVal > MStackData::BenchmarkStackPointerSave)
    // {
    //     SaveBenchmarkStack(((MStackData::BenchmarkMode + 1) % 2));
    // }

    // osData.debugTerminalWindow->Log("MSTACK: {}", to_string(tVal), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("MSTACK MAX: {}", to_string(MStackData::BenchmarkStackPointerSave), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // osData.debugTerminalWindow->Log("PC SPEAKER INDEX: {}", to_string(AudioDeviceStuff::currentRawAudioIndex), Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // if (osData.ac97Driver != NULL)
    //     osData.debugTerminalWindow->Log("AC97 OFF: {}", to_string(osData.ac97Driver->needManualRestart), Colors.yellow);
    // else
    //     osData.debugTerminalWindow->Log("<NO AC97>", Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // if (osData.ac97Driver != NULL)
    //     osData.debugTerminalWindow->Log("AC97 DO CHECK: {}", to_string(osData.ac97Driver->doCheck), Colors.yellow);
    // else
    //     osData.debugTerminalWindow->Log("<NO AC97>", Colors.yellow);
    

    
    // osData.debugTerminalWindow->renderer->Clear(
    //     osData.debugTerminalWindow->renderer->CursorPosition.x,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y,
    //     osData.debugTerminalWindow->renderer->CursorPosition.x + 240,
    //     osData.debugTerminalWindow->renderer->CursorPosition.y + 16,
    //     Colors.black);
    // if (osData.ac97Driver != NULL)
    //     osData.debugTerminalWindow->Log("AC97 DATA SRCS: {}", to_string(osData.ac97Driver->audioDestination->sources->GetCount()), Colors.yellow);
    // else
    //     osData.debugTerminalWindow->Log("<NO AC97>", Colors.yellow);
    
    
    if(++testCounterX >= testInterlace)
    {
        testCounterX = 0;
        if(++testCounterY >= testInterlace)
            testCounterY = 0;
    }   
}


void UpdateWindowBorder(Window* window)
{
    if (!window->ShowBorder)
        return;

    int x1 = window->Dimensions.x;
    int y1 = window->Dimensions.y;
    int x2 = x1 + window->Dimensions.width;
    int y2 = y1 + window->Dimensions.height;
    
    UpdatePointerRect(x1-1, y1-22, x1-1, y2);
    UpdatePointerRect(x2,   y1-22, x2,   y2);

    UpdatePointerRect(x1-1, y1-22, x2,   y1-22);
    UpdatePointerRect(x1-1, y1-1 ,x2,   y1-1);
    UpdatePointerRect(x1-1, y2, x2,   y2);

    UpdatePointerRect(x1,   y1-22, x2,   y1-1);
}

uint64_t RenderActualSquare(int _x1, int _y1, int _x2, int _y2)
{
    int64_t h = actualScreenFramebuffer->Height, w = actualScreenFramebuffer->Width, bpl = actualScreenFramebuffer->PixelsPerScanLine;


    if (_x1 < 0)
        _x1 = 0;
    // if (_x2 < 0)
    //     _x2 = 0;
    if (_y1 < 0)
        _y1 = 0;
    // if (_y2 < 0)
    //     _y2 = 0;

    // if (_x1 >= w)
    //     _x1 = w - 1;
    if (_x2 >= w)
        _x2 = w - 1;
    // if (_y1 >= h)
    //     _y1 = h - 1;
    if (_y2 >= h)
        _y2 = h - 1;

    if (_x1 > _x2)
        return 0;
    
    if (_y1 > _y2)
        return 0;
    

    //
    uint64_t counta = 0;
    uint64_t xdiff = _x2 - _x1;
    uint32_t** vPixel = (uint32_t**)pointerBuffer->BaseAddress + _x1 + w * _y1;
    uint32_t*  cPixel = (uint32_t*)  mainBuffer->BaseAddress + _x1 + w * _y1;
    uint32_t* aPixel = (uint32_t*) actualScreenFramebuffer->BaseAddress;

    int64_t wMinusSomeStuff = w - (xdiff+1);

    int64_t y1TimesBpl = (_y1 - 1) * bpl;
    // DRAW SQUARE
    for (int64_t y1 = _y1; y1 <= _y2; y1++)
    {
        y1TimesBpl += bpl;
        for (int64_t x1 = _x1; x1 <= _x2; x1++)
        {
            uint32_t col = **vPixel;
            if (*cPixel != col)
            {
                *cPixel = col;
                *(aPixel + (x1 + y1TimesBpl)) = col; //counta + 0xff111111;
                counta++;
                //osData.currentDisplay->UpdatePixel(x1, y1);
            }
            vPixel++;
            cPixel++;
        }
        vPixel += wMinusSomeStuff;
        cPixel += wMinusSomeStuff;
    }

    return counta;
}
