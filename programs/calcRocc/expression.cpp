#include "expression.hpp"

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

List<token> tokenize(const char *s) {
  List<token> tokens {};

  while (*s != '\0') {
    if (isDigit(*s)) {
      int num{};
      s += parseNum(s, &num);
      tokens.Add(token{'\0', num});
    } else if (isUnary(*s)) {
      tokens.Add(token{*s, 0});
      s++;
    } else if (*s == '(') {

      tokens.Add(token{'(', parensLength(s)});
      s++;
    } else if (*s == ')') {
      tokens.Add(token{')', 0});
      s++;
    } else {
      s++;
    }
  }

  tokens.Add(token{0, 0});

  return tokens;
}

int expr(List<token> &tokens, int prec, int index) {
  int rhs{};
  int lhs{};
  char op{};

  if (tokens[index].type == '(') {
    lhs = expr(tokens, 0, index + 1);
    index += tokens[index].val;
    op = tokens[index].type;
  } else {
    lhs = tokens[index].val;
    index++;
    op = tokens[index].type;
  }

  while (isUnary(tokens[index].type) && getPrec(tokens[index].type) >= prec) {
    index++;
    rhs = expr(tokens, getPrec(tokens[index].type) + 1, index);
    lhs = oper(op, lhs, rhs);
  }

  return lhs;
}