#pragma once
#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>
#include <libm/keyboard.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/gui/guiStuff/components/imageRect/imageRectangleComponent.h>
#include <libm/dialogStuff/dialogStuff.h>

void UpdateSizes();

void OnOpenClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
