#pragma once

#include "common_includes.hpp"

// buttons
extern ButtonComponent *plusBtn;
extern ButtonComponent *dashBtn;
extern ButtonComponent *starBtn;
extern ButtonComponent *slashBtn;
extern ButtonComponent *equalBtn;
extern ButtonComponent *dotBtn;
extern ButtonComponent *zeroBtn;
extern List<BaseComponent *> *keypad;

extern TextFieldComponent *screen;

// actions
void OnButtonClicked(void *func, ButtonComponent *btn, MouseClickEventInfo click);
void OnEqualClicked(void *func, ButtonComponent *btn, MouseClickEventInfo click);
bool SpecialKeyHandler(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info);
