#pragma once
#include <stdint.h>
#include <stddef.h>

#include "../rendering/framebuffer.h"
#include "../rendering/S-Font.h"

struct ENV_DATA
{
    Framebuffer* globalFrameBuffer;
    PSF1_FONT* globalFont;
};