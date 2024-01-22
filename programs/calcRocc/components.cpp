#include "common_includes.hpp"
#include "vars.hpp"

void style(GuiInstance *guiInstance)
{
    // + button
    guiInstance->CreateComponentWithId(1000, ComponentType::BUTTON);
    plusBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1000);
    plusBtn->position.x = 150;
    plusBtn->position.y = 200;
    _Free(plusBtn->textComp->text);
    plusBtn->textComp->text = StrCopy("+");
    plusBtn->size.FixedX = 50;
    plusBtn->size.FixedY = 50;
    plusBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;

    // - button
    guiInstance->CreateComponentWithId(1001, ComponentType::BUTTON);
    dashBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1001);
    dashBtn->position.x = 150;
    dashBtn->position.y = 150;
    _Free(dashBtn->textComp->text);
    dashBtn->textComp->text = StrCopy("-");
    dashBtn->size.FixedX = 50;
    dashBtn->size.FixedY = 50;
    dashBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;

    // * button
    guiInstance->CreateComponentWithId(1002, ComponentType::BUTTON);
    starBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1002);
    starBtn->position.x = 150;
    starBtn->position.y = 100;
    _Free(starBtn->textComp->text);
    starBtn->textComp->text = StrCopy("*");
    starBtn->size.FixedX = 50;
    starBtn->size.FixedY = 50;
    starBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;

    // / button
    guiInstance->CreateComponentWithId(1003, ComponentType::BUTTON);
    slashBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1003);
    slashBtn->position.x = 150;
    slashBtn->position.y = 50;
    _Free(slashBtn->textComp->text);
    slashBtn->textComp->text = StrCopy("/");
    slashBtn->size.FixedX = 50;
    slashBtn->size.FixedY = 50;
    slashBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;

    // = button
    guiInstance->CreateComponentWithId(1004, ComponentType::BUTTON);
    equalBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1004);
    equalBtn->position.x = 100;
    equalBtn->position.y = 200;
    _Free(equalBtn->textComp->text);
    equalBtn->textComp->text = StrCopy("=");
    equalBtn->size.FixedX = 50;
    equalBtn->size.FixedY = 50;
    equalBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnEqualClicked;

    // dot buttonn
    guiInstance->CreateComponentWithId(1005, ComponentType::BUTTON);
    dotBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1005);
    dotBtn->position.x = 0;
    dotBtn->position.y = 200;
    _Free(dotBtn->textComp->text);
    dotBtn->textComp->text = StrCopy(".");
    dotBtn->size.FixedX = 50;
    dotBtn->size.FixedY = 50;
    dotBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;

    // zero buttonn
    guiInstance->CreateComponentWithId(1006, ComponentType::BUTTON);
    zeroBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1006);
    zeroBtn->position.x = 50;
    zeroBtn->position.y = 200;
    _Free(zeroBtn->textComp->text);
    zeroBtn->textComp->text = StrCopy("0");
    zeroBtn->size.FixedX = 50;
    zeroBtn->size.FixedY = 50;
    zeroBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;

    // keypad
    keypad = new List<BaseComponent *>();
    for (int i = 1; i <= 9; ++i)
    {
        guiInstance->CreateComponentWithId(1006 + i, ComponentType::BUTTON);
        ButtonComponent *tempBtn =
            (ButtonComponent *)guiInstance->GetComponentFromId(1006 + i);
        keypad->Add(tempBtn);

        tempBtn = (ButtonComponent *)guiInstance->GetComponentFromId(1006 + i);
        tempBtn->position.x = ((i - 1) % 3) * 50;
        tempBtn->position.y = ((i - 1) / 3) * 50 + 50;
        _Free(tempBtn->textComp->text);
        tempBtn->textComp->text = StrCopy(to_string(i));
        tempBtn->size.FixedX = 50;
        tempBtn->size.FixedY = 50;
        tempBtn->OnMouseClickedCallBack = (void (*)(void*, BaseComponent*, MouseClickEventInfo))OnButtonClicked;
    }

    guiInstance->CreateComponentWithId(1020, ComponentType::TEXTFIELD);
    screen = (TextFieldComponent *)guiInstance->GetComponentFromId(1020);
    screen->position.x = 0;
    screen->position.y = 0;
    screen->size.FixedX = 200;
    screen->size.FixedY = 50;
    screen->AdvancedKeyHitCallBack = SpecialKeyHandler;
    screen->AdvancedKeyHitCallBackHelp = NULL;
}
