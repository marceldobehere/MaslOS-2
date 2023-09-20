#pragma once

#include <stdint.h>
#include <stddef.h>

struct Framebuffer
{
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
};

struct PointerBuffer
{
	uint32_t** BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
};