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
#include <libm/dialogStuff/dialogStuff.h>
#include <libm/memStuff.h>
#include <libm/stdio/stdio.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>

#include <libm/audio/audioInterface.h>



int main(int argc, char** argv);

void MainLoop();
void UpdateText();
void CheckMusic();

void LoadFile(const char* path);
bool LoadWavData(char* data, int64_t dataLen);

void OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
void OnPlayClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);