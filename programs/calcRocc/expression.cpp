#include "common_includes.hpp"
#include "expression.hpp"
#include "vars.hpp"

bool isDigit(const char c) { return c <= '9' && c >= '0'; }

bool isDigitToken(token t) { return t.type == '\0'; }

long parensLength(const char *s) {
  long matched = 0;
  long length = 1;

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

double getPrec(const char c) {
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

double parseNum(const char *s, double *des) {
  long num = 0;
  long index = 0;
  double floating{};
  bool isFloating = false;

  for (index = 0; isDigit(*s); ++index) {
    if (*(s + 1) == '.' && isFloating == false) {
      isFloating = true;
      floating = 0.1;
    } else if (isFloating == true) {
      floating *= 10;
    }
    num *= 10;
    num += *s - '0';
    s++;
  }

  *des = num / floating;
  return index;
}

double oper(const char op, double lhs, double rhs) {
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
  default:
    ParseError();
    return 0;
  }
}

token *mkToken(const char type, double val) {
  token *t = (token *)_Malloc(sizeof(token));
  t->type = type;
  t->val = val;
  return t;
}

void tokenize(List<token *> *tokens, const char *s) {
  double index = 0;
  while (*s != '\0') {
    if (isDigit(*s)) {
      double num = parseNum(s, &num);
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
    } else if (*s == ' ') {
      s++;
    } else {
      InvalidChar();
    }
    serialPrint("token: ");
    serialPrintLn(to_string(tokens->ElementAt(index)->val));
    index++;
  }

  tokens->Add(mkToken(0, 0));
}

double expr(List<token *> *tokens, long prec, long index) {
  serialPrint("parsing: ");
  serialPrintChar(tokens->ElementAt(index)->type - '0');
  serialPrintLn("");
  double rhs{};
  double lhs{};
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