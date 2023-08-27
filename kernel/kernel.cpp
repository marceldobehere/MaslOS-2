#include "kernel.h"
#include "kernelStuff/memory/memory.h"


 
void bootTest(Framebuffer fb, ACPI::RSDP2* rsdp, PSF1_FONT* psf1_font, MaslOsAssetStruct* assets, void* freeMemStart, void* extraMemStart, uint64_t freeMemSize, void* kernelStart, uint64_t kernelSize, void* kernelStartV)
{
    //MStackData::BenchmarkEnabled = false;
    BootInfo tempBootInfo;
    tempBootInfo.framebuffer = &fb;
    tempBootInfo.rsdp = rsdp;

    tempBootInfo.psf1_font = psf1_font;

    tempBootInfo.testImage = assets->testImage;
    tempBootInfo.bootImage = assets->bootImage;
    tempBootInfo.MButton = assets->MButton;
    tempBootInfo.MButtonS = assets->MButtonS;
    tempBootInfo.bgImage = assets->bgImage;
    tempBootInfo.maabZIP = assets->maabZIP;
    tempBootInfo.otherZIP = assets->otherZIP;

    tempBootInfo.mouseZIP = assets->mouseZIP;
    tempBootInfo.windowButtonZIP = assets->windowButtonZIP;
    tempBootInfo.windowIconsZIP = assets->windowIconsZIP;

    tempBootInfo.mMapStart = freeMemStart;
    tempBootInfo.m2MapStart = extraMemStart;
    tempBootInfo.mMapSize = freeMemSize;
    
    tempBootInfo.kernelStart = kernelStart;
    tempBootInfo.kernelSize = kernelSize;
    tempBootInfo.kernelStartV = kernelStartV;

    for (int y = 0; y < 100; y++)
        for (int x = 500; x < 600; x++)
            *(uint32_t*)((uint64_t)fb.BaseAddress + 4 * (x + y * fb.PixelsPerScanLine)) = (Colors.yellow * ((x + y + 1) % 2)) + (Colors.green * ((x + y) % 2));

    while (true);

    
    
    //boot(&tempBootInfo);
    return;
}
