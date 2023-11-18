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
#include <libm/list/list_guiBaseComp.h>

using namespace GuiComponentStuff;

void Restart();
void UpdateSizes();
void Frame();
bool CollisionCheck();



struct Pipe
{
    RectangleComponent* top;
    RectangleComponent* bottom;
    int x;
    int width;
    int height;
    int gap;
    int index;

    void Update();

    void Reset(bool pos);

    void Init(int index);
};

#define PIPE_COUNT 4
extern Pipe pipes[PIPE_COUNT];