#include "fps.h"
#include "main.h"

void PrintFPS(int fps, int aFps, int frameTime, int breakTime, int totalTime, uint64_t totalPixelCount, int frameCount)
{
    actualScreenRenderer->CursorPosition.x = 0;
    actualScreenRenderer->CursorPosition.y = actualScreenFramebuffer->Height - 64;
    
    actualScreenRenderer->Clear(
        0, 
        actualScreenRenderer->CursorPosition.y, 

        300, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );

    int tTime = totalTime;

    frameTime = (frameTime * 1000) / frameCount;
    breakTime = (breakTime * 1000) / frameCount;
    totalTime = (totalTime * 1000) / frameCount;
        
    actualScreenRenderer->Print("FPS: {}", to_string(aFps), Colors.yellow);
    actualScreenRenderer->Print(" ({})", to_string(fps), Colors.yellow);
    actualScreenRenderer->Print(" ({} /", to_string(frameTime), Colors.yellow);
    actualScreenRenderer->Print(" {} /", to_string(breakTime), Colors.yellow);
    actualScreenRenderer->Print(" {})", to_string((totalTime)), Colors.yellow);


    actualScreenRenderer->CursorPosition.x = 300;
    actualScreenRenderer->Clear(
        300, 
        actualScreenRenderer->CursorPosition.y, 

        500, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );
    actualScreenRenderer->Print("MALLOC: {}", to_string(Heap::GlobalHeapManager->_usedHeapCount), Colors.yellow);  


    actualScreenRenderer->CursorPosition.x = 500;
    actualScreenRenderer->Clear(
        500, 
        actualScreenRenderer->CursorPosition.y, 

        880, 
        actualScreenRenderer->CursorPosition.y + 16, 
        Colors.black
    );
    actualScreenRenderer->Print("PIXELS PER FRAME: {}", to_string(totalPixelCount / frameCount), Colors.yellow);

    totalTime = tTime;

    actualScreenRenderer->Print(" ({} PPS)", to_string((totalPixelCount * 1000) / totalTime), Colors.yellow);
}