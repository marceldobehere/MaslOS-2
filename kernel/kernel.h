#pragma once

#define _KERNEL_SRC

#include "rendering/Framebuffer.h"
#include "devices/acpi/acpi.h"
#include "kernelStuff/kernelFiles/kernelFiles.h"
#include "kernelStuff/kernelFiles/ImageStruct.h"
#include "kernelStuff/kernelFiles/FileStruct.h"
#include "kernelStuff/kernelFiles/ZIPStruct.h"

#include <libm/rendering/S-Font.h>

struct MaslOsAssetStruct
{
    // Images
    kernelFiles::ImageFile* bgImage;
    kernelFiles::ImageFile* testImage;
	kernelFiles::ImageFile* bootImage;
	kernelFiles::ImageFile* MButton;
	kernelFiles::ImageFile* MButtonS;

    // Zips
    kernelFiles::ZIPFile* mouseZIP;
	kernelFiles::ZIPFile* windowButtonZIP;
	kernelFiles::ZIPFile* windowIconsZIP;

    kernelFiles::ZIPFile* maabZIP;
    kernelFiles::ZIPFile* otherZIP;

    // Modules
    kernelFiles::DefaultFile* programs;
    //kernelFiles::DefaultFile* nothingDoer;

    // EFI Mem?

};

struct limineSmpInfo {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t reserved;
    uint64_t* goto_address;
    uint64_t extra_argument;
};

struct limineSmpResponse {
    uint64_t revision;
    uint32_t flags;
    uint32_t bsp_lapic_id;
    uint64_t cpu_count;
    limineSmpInfo** cpus;
};

void boot(void* bootInfo);


void bootTest(Framebuffer fb, ACPI::RSDP2* rsdp, PSF1_FONT* psf1_font, MaslOsAssetStruct* assets, void* freeMemStart, void* extraMemStart, uint64_t freeMemSize, void* kernelStart, uint64_t kernelSize, void* kernelStartV, limineSmpResponse* smpData);

// void RenderLoop();
// void RecoverDed();