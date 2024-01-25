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
#include <libm/memStuff.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/list/list_guiBaseComp.h>

using namespace GuiComponentStuff;

void WinCheck();
void UpdateSizes();
void Restart();
void OnRestartClicked(void* bruh, BaseComponent* btn, MouseClickEventInfo click);
void OnFieldClicked(void* bruh, MouseClickEventInfo click);