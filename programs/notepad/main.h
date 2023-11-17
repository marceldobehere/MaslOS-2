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
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>



void UpdateSizes();

void OnSaveClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
void OnSaveAsClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
void OnLoadClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info);

void SaveInto(const char* path);
void LoadFrom(const char* path);