#pragma once

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/base/baseComponent.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>

using namespace GuiComponentStuff;

extern ButtonComponent *plusBtn;
extern ButtonComponent *dashBtn;
extern ButtonComponent *starBtn;
extern ButtonComponent *slashBtn;
extern ButtonComponent *equalBtn;
extern ButtonComponent *dotBtn;
extern ButtonComponent *zeroBtn;
extern List<BaseComponent *> *keypad;;

extern TextFieldComponent *screen;