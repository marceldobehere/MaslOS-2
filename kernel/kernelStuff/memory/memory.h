#pragma once

#include "../../rendering/BasicRenderer.h"
#include "efiMemory.h"
#include <libm/rendering/Cols.h>
#include "../kernelFiles/FileStruct.h"  //kernelFiles::
#include "../kernelFiles/ZIPStruct.h"
#include "../../devices/acpi/acpi.h"
#include "../../kernel.h"


struct BootInfo
{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	kernelFiles::DefaultFile* bootImage;
	kernelFiles::DefaultFile* programs;
	//kernelFiles::DefaultFile* nothingDoer;
	//EFI_MEMORY_DESCRIPTOR* mMap;
	// uint64_t mMapSize;
	// uint64_t mMapDescSize;
    ACPI::RSDP2* rsdp;
	void* mMapStart;
	void* m2MapStart;
	uint64_t mMapSize;
	void* kernelStart;
	uint64_t kernelSize;
	void* kernelStartV;
	limineSmpResponse* smpData;
};

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);

void PrintEFIMemData(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize, BasicRenderer* temp );

void _memset(void* start, uint8_t value, uint64_t num);

void _memcpy(const void* src, void* dest, uint64_t size);

int _memcmp(const void* src, const void* dest, int amt);
