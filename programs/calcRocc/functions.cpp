#include "common_includes.hpp"
#include "expression.hpp"
#include "vars.hpp"

void OnButtonClicked(void *func, ButtonComponent *btn,
                     MouseClickEventInfo click) {
  screen->textComp->text =
      StrCombine(screen->textComp->text, btn->textComp->text);
}

void OnEqualClicked(void *func, ButtonComponent *btn,
                    MouseClickEventInfo click) {
  List<void *> *tokens = new List<void *>;
  tokenize(tokens, screen->textComp->text);
  double res = expr(tokens, 0, 0);
  _Free(screen->textComp->text);
  screen->textComp->text = StrCopy(to_string(res));
}

void ParseError() {
  screen->bgCol = Colors.bred;
  screen->textComp->text = StrCopy("wrong equation!");
}

void InvalidChar() {
  screen->bgCol = Colors.bred;
  screen->textComp->text = StrCopy("invalid input!");
}
