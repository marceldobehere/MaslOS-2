#include <libm/syscallManager.h>
#include <libm/env/env.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/cstr.h>
#include <libm/list/list_window.h>
#include <libm/memStuff.h>



void DrawFrame();

TempRenderer* actualScreenRenderer;
Framebuffer* actualScreenFramebuffer;
Framebuffer* mainBuffer;
PointerBuffer* pointerBuffer;
uint32_t bgCol = Colors.gray;

List<Window*>* windows;

int main()
{
    serialPrintLn("Starting Desktop");

    programSetPriority(2);

    ENV_DATA* env = getEnvData();

    programWait(500);

    actualScreenFramebuffer = env->globalFrameBuffer;
    actualScreenRenderer = new TempRenderer(actualScreenFramebuffer, env->globalFont);
    
    {
        mainBuffer = new Framebuffer();
        mainBuffer->Width = actualScreenFramebuffer->Width;
        mainBuffer->Height = actualScreenFramebuffer->Height;
        mainBuffer->PixelsPerScanLine = mainBuffer->Width;
        mainBuffer->BufferSize = mainBuffer->Width * mainBuffer->Height * 4;
        mainBuffer->BaseAddress = _Malloc(mainBuffer->BufferSize, "Main Buffer");
        _memset(mainBuffer->BaseAddress, 0, mainBuffer->BufferSize);
    }

    {
        pointerBuffer = new PointerBuffer();
        pointerBuffer->Width = actualScreenFramebuffer->Width;
        pointerBuffer->Height = actualScreenFramebuffer->Height;
        pointerBuffer->BufferSize = pointerBuffer->Width * pointerBuffer->Height * sizeof(uint32_t*);
        pointerBuffer->BaseAddress = (uint32_t**)_Malloc(pointerBuffer->BufferSize, "Pointer Buffer");
        for (int y = 0; y < pointerBuffer->Height; y++)
            for (int x = 0; x < pointerBuffer->Width; x++)
                pointerBuffer->BaseAddress[x + y * pointerBuffer->Width] = &bgCol;
    }
    
    windows = new List<Window*>(5);

    Window* window = new Window(50, 0, 200, 200, "Test Window 1");
    windows->Add(window);
    


    serialPrintLn("Starting Main Loop");
    
    const int frameCount = 30;

    while (true)
    {
        uint64_t startTime = envGetTimeMs();
        
        for (int i = 0; i < frameCount; i++)
        {
            DrawFrame();

            programWait(10);
            //programYield();
        }

        uint64_t endTime = envGetTimeMs();

        uint64_t frameTime = endTime - startTime;

        int fps = (int)((frameCount * 1000) / frameTime);

        actualScreenRenderer->CursorPosition.x = 0;
        actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 64;
        
        actualScreenRenderer->Clear(
            0, 
            actualScreenRenderer->CursorPosition.y, 

            160, 
            actualScreenRenderer->CursorPosition.y + 16, 
            Colors.black
        );

        actualScreenRenderer->Print("FPS: {}", to_string(fps), Colors.yellow);
    }

    return 0;
}

void DrawFrame()
{
    uint32_t rndCol = (uint32_t)RND::RandomInt();

    uint32_t* mainData = (uint32_t*)mainBuffer->BaseAddress;
    uint32_t* actualData = (uint32_t*)actualScreenFramebuffer->BaseAddress;
    uint32_t** pointerData = (uint32_t**)pointerBuffer->BaseAddress;

    for (int y = 0; y < pointerBuffer->Height; y++)
        for (int x = 0; x < pointerBuffer->Width; x++)
        {
            uint32_t tempCol = *pointerData[x + y * pointerBuffer->Width];

            if (tempCol != mainData[x + y * mainBuffer->PixelsPerScanLine])
            {
                actualData[x + y * actualScreenFramebuffer->PixelsPerScanLine] = tempCol;
                mainData[x + y * mainBuffer->PixelsPerScanLine] = tempCol;
            }
        }

    actualScreenRenderer->Clear(0, 0, 200, 200, rndCol);
}


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