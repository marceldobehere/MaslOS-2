#include "expression.hpp"

#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/heap/heap.h>
#include <libm/rendering/Cols.h>
#include <libm/syscallManager.h>
#include <libm/wmStuff/wmStuff.h>

using namespace GuiComponentStuff;

int main(int argc, char **argv) {

  initWindowManagerStuff();

  Window *window = requestWindow();
  if (window == NULL) {
    return -1;
  }
  window->Resizeable = false;

  _Free(window->Title);
  window->Title = StrCopy("calcRocc");

  window->Dimensions.width = 200;
  window->Dimensions.height = 250;

  setWindow(window);

  GuiInstance *guiInstance = new GuiInstance(window);
  guiInstance->Init();

  // + button
  guiInstance->CreateComponentWithId(1000, ComponentType::BUTTON);
  ButtonComponent *plusBtn;
  plusBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1000);
  plusBtn->position.x = 150;
  plusBtn->position.y = 200;
  _Free(plusBtn->textComp->text);
  plusBtn->textComp->text = StrCopy("+");
  plusBtn->size.FixedX = 50;
  plusBtn->size.FixedY = 50;
  // plusBtn->OnMouseClickHelp = NULL;
  // plusBtn->OnMouseClickedCallBack = OnplusClicked;

  // - button
  ButtonComponent *dashBtn;
  guiInstance->CreateComponentWithId(1001, ComponentType::BUTTON);
  dashBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1001);
  dashBtn->position.x = 150;
  dashBtn->position.y = 150;
  _Free(dashBtn->textComp->text);
  dashBtn->textComp->text = StrCopy("-");
  dashBtn->size.FixedX = 50;
  dashBtn->size.FixedY = 50;
  // dashBtn->OnMouseClickHelp = NULL;
  // dashBtn->OnMouseClickedCallBack = OndashClicked;

  // * button
  guiInstance->CreateComponentWithId(1002, ComponentType::BUTTON);
  ButtonComponent *starBtn;
  starBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1002);
  starBtn->position.x = 150;
  starBtn->position.y = 100;
  _Free(starBtn->textComp->text);
  starBtn->textComp->text = StrCopy("*");
  starBtn->size.FixedX = 50;
  starBtn->size.FixedY = 50;
  // starBtn->OnMouseClickHelp = NULL;
  // starBtn->OnMouseClickedCallBack = OnstarClicked;

  // / button
  guiInstance->CreateComponentWithId(1003, ComponentType::BUTTON);
  ButtonComponent *slashBtn;
  slashBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1003);
  slashBtn->position.x = 150;
  slashBtn->position.y = 50;
  _Free(slashBtn->textComp->text);
  slashBtn->textComp->text = StrCopy("/");
  slashBtn->size.FixedX = 50;
  slashBtn->size.FixedY = 50;
  // slashBtn->OnMouseClickHelp = NULL;
  // slashBtn->OnMouseClickedCallBack = OnslashClicked;

  // = button
  guiInstance->CreateComponentWithId(1004, ComponentType::BUTTON);
  ButtonComponent *equalBtn;
  equalBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1004);
  equalBtn->position.x = 100;
  equalBtn->position.y = 200;
  _Free(equalBtn->textComp->text);
  equalBtn->textComp->text = StrCopy("=");
  equalBtn->size.FixedX = 50;
  equalBtn->size.FixedY = 50;
  // equalBtn->OnMouseClickHelp = NULL;
  // equalBtn->OnMouseClickedCallBack = OnequalClicked;

  // dot buttonn
  guiInstance->CreateComponentWithId(1005, ComponentType::BUTTON);
  ButtonComponent *dotBtn;
  dotBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1005);
  dotBtn->position.x = 0;
  dotBtn->position.y = 0;
  _Free(dotBtn->textComp->text);
  dotBtn->textComp->text = StrCopy(",");
  dotBtn->size.FixedX = 50;
  dotBtn->size.FixedY = 50;
  // dotBtn->OnMouseClickHelp = NULL;
  // dotBtn->OnMouseClickedCallBack = OndotClicked;

  // zero buttonn
  guiInstance->CreateComponentWithId(1006, ComponentType::BUTTON);
  ButtonComponent *zeroBtn;
  zeroBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1005);
  zeroBtn->position.x = 50;
  zeroBtn->position.y = 200;
  _Free(zeroBtn->textComp->text);
  zeroBtn->textComp->text = StrCopy("0");
  zeroBtn->size.FixedX = 50;
  zeroBtn->size.FixedY = 50;
  // zeroBtn->OnMouseClickHelp = NULL;
  // zeroBtn->OnMouseClickedCallBack = OnzeroClicked;

  // keypad
  List<BaseComponent *> *keypad = new List<BaseComponent *>();
  for (int i = 1; i <= 9; ++i) {
    guiInstance->CreateComponentWithId(1006 + i, ComponentType::BUTTON);
    ButtonComponent *tempBtn =
        (ButtonComponent *)guiInstance->GetComponentFromId(1006 + i);
    keypad->Add(tempBtn);

    tempBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1006 + i);
    tempBtn->position.x = ((i-1) % 3) * 50;
    tempBtn->position.y = ((i-1) / 3) * 50 + 50;
    _Free(tempBtn->textComp->text);
    tempBtn->textComp->text = StrCopy(to_string(i));
    tempBtn->size.FixedX = 50;
    tempBtn->size.FixedY = 50;
    // tempBtn->OnMouseClickHelp = NULL;
    // tempBtn->OnMouseClickedCallBack = OnzeroClicked;
  }

  while (!CheckForWindowClosed(window)) {
    guiInstance->Update();
    guiInstance->Render(false);

    programWaitMsg();
  }

  return 0;
}
