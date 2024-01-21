#include "expression.hpp"
#include "vars.hpp"

#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/gui/guiStuff/components/base/baseComponent.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>
#include <libm/syscallManager.h>

void OnButtonClicked(void *func, ButtonComponent *btn,
                     MouseClickEventInfo click) {
  screen->textComp->text =
      StrCombine(screen->textComp->text, btn->textComp->text);
}

void OnEqualClicked(void *func, ButtonComponent *btn,
                    MouseClickEventInfo click) {
  serialPrintLn("now on to tokenizing!");
  List<token *> *tokens = new List<token *>();
  tokenize(tokens, screen->textComp->text);
  serialPrintLn("now on to parsing!");
  int res = expr(tokens, 0, 0);
  serialPrint("on to printing!");
  serialPrintLn(to_string(res));
  _Free(screen->textComp->text);
  screen->textComp->text = StrCopy(to_string(res));
}
