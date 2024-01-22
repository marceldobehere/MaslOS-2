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
extern void OnButtonClicked(void *func, ButtonComponent *btn,
                            MouseClickEventInfo click);

extern void OnEqualClicked(void *func, ButtonComponent *btn,
                           MouseClickEventInfo click);

extern void ParseError();

extern void InvalidChar();
