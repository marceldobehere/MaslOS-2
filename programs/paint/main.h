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
#include <libm/gui/guiStuff/components/canvas/canvasComponent.h>

#include <libm/list/list_guiBaseComp.h>


using namespace GuiComponentStuff;

void MainLoop();
void HandleFrame();
void UpdateSizes();

void OnOpenClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click);
void OnSaveClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click);
void OnSaveAsClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click);
void OnClearClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click);
void OnCanvasClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click);
void OnColorClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click);