#include "common_includes.hpp"
#include "expression.hpp"
#include "vars.hpp"
#include <libm/keyboard.h>

void OnButtonClicked(void *func, ButtonComponent *btn, MouseClickEventInfo click)
{
    screen->textComp->text =
        StrCombine(screen->textComp->text, btn->textComp->text);
}

void FreeTokens(List<void *> *tokens)
{
    for (int i = 0; i < tokens->GetCount(); i++)
        _Free(tokens->ElementAt(i));
    tokens->Free();
    _Free(tokens);
}

void OnEqualClicked(void *func, ButtonComponent *btn, MouseClickEventInfo click)
{
    if (StrEquals(screen->textComp->text, "invalid input!"))
    {
        screen->bgCol = Colors.white;
        _Free(screen->textComp->text);
        screen->textComp->text = StrCopy("");
        return;
    }

    List<void *>* tokens = new List<void *>();
    if (!tokenize(tokens, screen->textComp->text))
    {
        screen->bgCol = Colors.bred;
        _Free(screen->textComp->text);
        screen->textComp->text = StrCopy("invalid input!");
        FreeTokens(tokens);
        return;
    }
    
    if (tokens->GetCount() == 0 || StrEquals(screen->textComp->text, ""))
    {
        screen->bgCol = Colors.white;
        _Free(screen->textComp->text);
        screen->textComp->text = StrCopy("");
        FreeTokens(tokens);
        return;
    }

    double res = 0;
    if (parse(tokens, 0, 0, &res))
    {
        screen->bgCol = Colors.white;
        _Free(screen->textComp->text);
        screen->textComp->text = StrCopy(to_string(res));
    }
    else
    {
        screen->bgCol = Colors.bred;
        _Free(screen->textComp->text);
        screen->textComp->text = StrCopy("wrong equation!");
    }
    FreeTokens(tokens);
}

bool SpecialKeyHandler(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info)
{
    if (info.Scancode == Key_Enter)
    {
        OnEqualClicked(NULL, NULL, MouseClickEventInfo(Position(0,0), false, false, false));
        return false;
    }
    
    return true;
}