#include "main.h"
#include <cstdint>
#include <libm/rendering/virtualRenderer.h>
#include <libm/cstrTools.h>
// #include "wmStuff.h"
// #include "mouseStuff.h"
#include <libm/msgPackets/mousePacket/mousePacket.h>
#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/windowBufferUpdatePacket/windowBufferUpdatePacket.h>
#include <libm/msgPackets/windowObjPacket/windowObjPacket.h>
#include <libm/images/bitmapImage.h>
// #include "taskbarConst.h"
#include <libm/fsStuff/extra/fsExtra.h>
#include <libm/keyboard.h>

TempRenderer* actualScreenRenderer;
Framebuffer* actualScreenFramebuffer;
Framebuffer* mainBuffer;
PointerBuffer* pointerBuffer;

uint32_t defaultBackgroundColor = Colors.black;
bool drawBackground = false;
Framebuffer* backgroundImage = NULL;

Framebuffer* taskbar;

List<void*>* windowIconEntries;
List<Window*>* windows;
List<Window*>* windowsToDelete;
Window* activeWindow;
Window* currentActionWindow;
Window* startMenuWindow;
uint64_t startMenuPid = 0;

ImageStuff::BitmapImage* windowButtonIcons[countOfButtonIcons];

ImageStuff::BitmapImage* internalWindowIcons[countOfWindowIcons];

bool MouseButtons[3] = { false, false, false };
MPoint MousePosition;
MPoint oldMousePos;



Queue<WindowUpdate>* ScreenUpdates;
List<GenericMessagePacket*>* tempPackets;

uint64_t lastFrameTime = 0;

#include <libm/zips/basicZip.h>

#include "fps.h"
#include "rendering.h"
#include "events.h"
#include "wmStuff.h"

void InitStuff()
{
    ENV_DATA* env = getEnvData();

    windowIconEntries = new List<void*>();

    actualScreenFramebuffer = env->globalFrameBuffer;
    actualScreenRenderer = new TempRenderer(actualScreenFramebuffer, env->globalFont);
    VirtualRenderer::psf1_font = env->globalFont;

    {
        mainBuffer = new Framebuffer();
        mainBuffer->Width = actualScreenFramebuffer->Width;
        mainBuffer->Height = actualScreenFramebuffer->Height;
        mainBuffer->PixelsPerScanLine = mainBuffer->Width;
        mainBuffer->BufferSize = mainBuffer->Width * mainBuffer->Height * 4;
        mainBuffer->BaseAddress = _Malloc(mainBuffer->BufferSize, "Main Buffer");
        _memset(mainBuffer->BaseAddress, 0, mainBuffer->BufferSize);
    }

    // {
    //     pointerBuffer = new PointerBuffer();
    //     pointerBuffer->Width = actualScreenFramebuffer->Width;
    //     pointerBuffer->Height = actualScreenFramebuffer->Height;
    //     pointerBuffer->BufferSize = pointerBuffer->Width * pointerBuffer->Height * sizeof(uint32_t*);
    //     pointerBuffer->BaseAddress = (uint32_t**)_Malloc(pointerBuffer->BufferSize, "Pointer Buffer");
    //     for (int y = 0; y < pointerBuffer->Height; y++)
    //         for (int x = 0; x < pointerBuffer->Width; x++)
    //             pointerBuffer->BaseAddress[x + y * pointerBuffer->Width] = &defaultBackgroundColor;
    // }
    
    // {
    //     taskbar = new Framebuffer();
    //     taskbar->Width = actualScreenFramebuffer->Width;
    //     taskbar->Height = TASKBAR_HEIGHT;
    //     taskbar->PixelsPerScanLine = taskbar->Width;
    //     taskbar->BufferSize = taskbar->Width * taskbar->Height * 4;
    //     taskbar->BaseAddress = _Malloc(taskbar->BufferSize, "Taskbar Buffer");
    //     for (int y = 0; y < taskbar->Height; y++)
    //         for (int x = 0; x < taskbar->Width; x++)
    //             ((uint32_t*)taskbar->BaseAddress)[x + y * taskbar->PixelsPerScanLine] = Colors.dblue;
    // }

    // activeWindow = NULL;
    // currentActionWindow = NULL;

    // MousePosition.x = 0;
    // MousePosition.y = 0;

    // oldMousePos = MousePosition;
    // oldMousePos.x += 10;
    

    // windows = new List<Window*>(5);
    // windowsToDelete = new List<Window*>(3);
    // tempPackets = new List<GenericMessagePacket*>(5);

    // ScreenUpdates = new Queue<WindowUpdate>(20);

    InitEvents();

    // startMenuWindow = NULL;
    // startMenuPid = 0;

    // lastFrameTime = envGetTimeMs();



    // const char* bgPath = "bruh:modules/desktop/assets/background.mbif";

    // {
    //     char* buf;
    //     uint64_t size = 0;
    //     if (fsReadFile(bgPath, (void**)&buf, &size))
    //     {
    //         ImageStuff::BitmapImage* img = ImageStuff::ConvertBufferToBitmapImage(buf, size);
    //         if (img != NULL)
    //         {
    //             backgroundImage = new Framebuffer();
    //             backgroundImage->Width = img->width;
    //             backgroundImage->Height = img->height;
    //             backgroundImage->PixelsPerScanLine = backgroundImage->Width;
    //             backgroundImage->BufferSize = backgroundImage->Width * backgroundImage->Height * 4;
    //             backgroundImage->BaseAddress = _Malloc(backgroundImage->BufferSize, "Background Image Buffer");
    //             _memcpy(img->imageBuffer, backgroundImage->BaseAddress, backgroundImage->BufferSize);
    //             _Free(img->imageBuffer);
    //             _Free(img);
    //             drawBackground = true;
    //         }
    //         _Free(buf);
    //     }
    // }

    // const char* windowButtonPath = "bruh:modules/desktop/assets/windowButtons.mbzf";
    // {
    //     char* buf;
    //     uint64_t size = 0;
    //     if (fsReadFile(windowButtonPath, (void**)&buf, &size))
    //     {
    //         ZipStuff::ZIPFile* zip = ZipStuff::ZIP::GetZIPFromBuffer(buf, size);

    //         for (int i = 0; i < countOfButtonIcons; i++)
    //             windowButtonIcons[i] = ImageStuff::ConvertFileToBitmapImage(ZipStuff::ZIP::GetFileFromFileName(zip, windowButtonIconNames[i]));
    //     }
    //     else
    //     {
    //         for (int i = 0; i < countOfButtonIcons; i++)    
    //             windowButtonIcons[i] = NULL;
    //     }
    // }

    // const char* windowIconsPath = "bruh:modules/desktop/assets/windowIcons.mbzf";
    // {
    //     char* buf;
    //     uint64_t size = 0;
    //     if (fsReadFile(windowIconsPath, (void**)&buf, &size))
    //     {
    //         ZipStuff::ZIPFile* zip = ZipStuff::ZIP::GetZIPFromBuffer(buf, size);

    //         for (int i = 0; i < countOfWindowIcons; i++)
    //             internalWindowIcons[i] = ImageStuff::ConvertFileToBitmapImage(ZipStuff::ZIP::GetFileFromFileName(zip, windowIconNames[i]));
    //     }
    //     else
    //     {
    //         for (int i = 0; i < countOfWindowIcons; i++)    
    //             internalWindowIcons[i] = NULL;
    //     }
    // }

    // mouseZIP = NULL;
    // {
    //     char* buf;
    //     uint64_t size = 0;
    //     if (fsReadFile("bruh:modules/desktop/assets/mouse.mbzf", (void**)&buf, &size))
    //     {
    //         mouseZIP = ZipStuff::ZIP::GetZIPFromBuffer(buf, size);
    //         //_Free(buf);
    //     }
    // }

    // ImageStuff::BitmapImage* mButton = NULL;
    // const char* mButtonPath = "bruh:modules/desktop/assets/MButton.mbif";
    // {
    //     char* buf;
    //     uint64_t size = 0;
    //     if (fsReadFile(mButtonPath, (void**)&buf, &size))
    //     {
    //         mButton = ImageStuff::ConvertBufferToBitmapImage(buf, size);
    //     }
    // }

    // ImageStuff::BitmapImage* mButtonS = NULL;
    // const char* mButtonSPath = "bruh:modules/desktop/assets/MButtonS.mbif";
    // {
    //     char* buf;
    //     uint64_t size = 0;
    //     if (fsReadFile(mButtonSPath, (void**)&buf, &size))
    //     {
    //         mButtonS = ImageStuff::ConvertBufferToBitmapImage(buf, size);
    //     }
    // }

    // Taskbar::InitTaskbar(mButton, mButtonS);
}



int main(int argc, char** argv)
{
    serialPrintLn("Starting Desktop");
    programSetPriority(2);
    InitStuff();

    activeWindow = NULL;

    // Clear and Redraw
    DoFrame();

    serialPrintLn("Starting Main Loop");
    
    const int frameCount = 60;
    while (true)
    {
        uint64_t frameTime = 0;
        uint64_t breakTime = 0;
        uint64_t totalPixelCount = 0;
        
        uint64_t _startTime = envGetTimeMs();
        for (int i = 0; i < frameCount; i++)
        {
            uint64_t startTime = envGetTimeMs();
            totalPixelCount += DoFrame();
            uint64_t endTime = envGetTimeMs();

            frameTime += endTime - startTime;
            programWaitMsg();

            uint64_t endTime2 = envGetTimeMs();
            breakTime += endTime2 - endTime;
        }

        

        uint64_t _endTime = envGetTimeMs();

        uint64_t totalTime = _endTime - _startTime;

        if (frameTime == 0)
            frameTime = 1;
        if (totalTime == 0)
            totalTime = 1;
        int fps = (int)((frameCount * 1000) / frameTime);
        int aFps = (int)((frameCount * 1000) / totalTime);


        PrintFPS(fps, aFps, frameTime, breakTime, totalTime, totalPixelCount, frameCount);
    }

    return 0;
}

int fC = 0;
uint64_t DoFrame() {
    // Handle Events
    HandleEvents();

    // Checks but not every frame
    fC++;
    if ((fC & 7) == 0) {
        fC = 0;
        CheckForDeadWindows();
        CheckForStartMenu();
    }

    // Render Updates
    return DrawFrame();
}