#include "expression.hpp"
#include "common_includes.hpp"
#include "vars.hpp"

bool isDigit(const char c) { return c <= '9' && c >= '0'; }

bool isDigitToken(Token t) { return t.type == '\0'; }

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

long parseNum(const char *s, double *des) {
  double num = 0.0;
  long index = 0;
  long floatingCount = 0;
  double floatingNum = 0.0;

  for (index = 0; isDigit(*s) || *s == '.'; ++index) {
    if (*s == '.') {
      floatingNum = 1.0;
      s++;
      index++;
      floatingCount += parseNum(s, &floatingNum);
      break;
    }

    num *= 10.0;
    num += (double)(*s - '0');
    s++;
  }

  for (int i = floatingCount; i; i--) {
    floatingNum /= 10;
  }

  *des = num + floatingNum;
  return index + floatingCount;
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

void *mkToken(const char type, double val) {
  Token *t = new Token();
  t->type = type;
  t->val = val;
  return t;
}

void tokenize(List<void *> *tokens, const char *s) {
  while (*s != '\0') {
    if (isDigit(*s)) {
      double num = 0.0;
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
    } else if (*s == ' ') {
      s++;
    } else {
      InvalidChar();
      s++;
    }
  }

  tokens->Add(mkToken(0, 0));
}

double parse(List<void *> *tokens, long prec, long index) {
  double rhs{};
  double lhs{};
  char op{};

  if (((Token *)tokens->ElementAt(index))->type == '(') {
    lhs = parse(tokens, 0, index + 1);
    index += ((Token *)tokens->ElementAt(index))->val;
    op = ((Token *)tokens->ElementAt(index))->type;
  } else {
    lhs = ((Token *)tokens->ElementAt(index))->val;
    index++;
    op = ((Token *)tokens->ElementAt(index))->type;
  }

  while (isUnary(((Token *)tokens->ElementAt(index))->type) &&
         getPrec(((Token *)tokens->ElementAt(index))->type) >= prec) {
    index++;
    rhs = parse(tokens, getPrec(((token *)tokens->ElementAt(index))->type) + 1,
                index);
    lhs = oper(op, lhs, rhs);
  }
  return lhs;
}