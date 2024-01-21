#include "expression.hpp"

#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/heap/heap.h>
#include <libm/memStuff.h>
#include <libm/syscallManager.h>

bool isDigit(const char c) { return c <= '9' && c >= '0'; }

bool isDigitToken(token t) { return t.type == '\0'; }

int parensLength(const char *s) {
  int matched = 0;
  int length = 1;

  if (*s == '(') {
    matched++;
    s++;
  }

  while (matched != 0) {
    if (*s == '(') {
      matched++;
    } else if (*s == ')') {
      matched--;
    }

    length++;
    s++;
  }

  return length;
}

bool isUnary(const char c) {
  switch (c) {
  case '+':
  case '-':
  case '/':
  case '*':
  case ')':
    return true;
  default:
    return false;
  }
}

int getPrec(const char c) {
  switch (c) {
  case '+':
    return 1;
  case '-':
    return 1;
  case '/':
    return 2;
  case '*':
    return 2;
  default:
    return 0;
  }
}

int parseNum(const char *s, int *des) {
  int num = 0;
  int index = 0;

  for (index = 0; isDigit(*s); ++index) {
    num *= 10;
    num += *s - '0';
    s++;
  }

  *des = num;
  return index;
}

int oper(const char op, int lhs, int rhs) {
  switch (op) {
  case '+':
    return lhs + rhs;
  case '-':
    return lhs - rhs;
  case '*':
    return lhs * rhs;
  case '/':
    if (rhs != 0) {
      return lhs / rhs;
    case ')':
      return lhs;
    }
  }
}

token *mkToken(const char type, int val) {
  token *t = (token *)_Malloc(sizeof(token));
  t->type = type;
  t->val = val;
  return t;
}

void tokenize(List<token *> *tokens, const char *s) {
  int index = 0;
  while (*s != '\0') {
    if (isDigit(*s)) {
      int num{};
      s += parseNum(s, &num);
      tokens->Add(mkToken('\0', num));
    } else if (isUnary(*s)) {

      tokens->Add(mkToken(*s, 0));
      s++;
    } else if (*s == '(') {

      tokens->Add(mkToken('(', parensLength(s)));
      s++;
    } else if (*s == ')') {
      tokens->Add(mkToken(')', 0));
      s++;
    } else {
      s++;
    }
    serialPrint("token: ");
    serialPrintLn(to_string(tokens->ElementAt(index)->val));
    index++;
  }

  tokens->Add(mkToken(0, 0));
}

int expr(List<token *> *tokens, int prec, int index) {
  serialPrint("parsing: ");
  serialPrintChar(tokens->ElementAt(index)->type - '0');
  serialPrint(to_string(tokens->ElementAt(index)->val));
  serialPrintLn("");
  int rhs{};
  int lhs{};
  char op{};

  if (tokens->ElementAt(index)->type == '(') {
    lhs = expr(tokens, 0, index + 1);
    index += tokens->ElementAt(index)->val;
    op = tokens->ElementAt(index)->type;
  } else {
    lhs = tokens->ElementAt(index)->val;
    index++;
    op = tokens->ElementAt(index)->type;
  }

  while (isUnary(tokens->ElementAt(index)->type) &&
         getPrec(tokens->ElementAt(index)->type) >= prec) {
    index++;
    rhs = expr(tokens, getPrec(tokens->ElementAt(index)->type) + 1, index);
    lhs = oper(op, lhs, rhs);
  }

  serialPrintLn("end of Parsing");
  return lhs;
}