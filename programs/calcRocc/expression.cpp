#include <cstdio>
#include <vector>

struct token {
  char type;
  int val;
};

int equation(std::vector<token> &tokens, int prec, int index);
int expr(std::vector<token> &tokens, int prec, int index);

bool isDigit(const char c) {
  if (c <= '9' && c >= '0') {
    return true;
  } else {
    return false;
  }
}

bool isUnary(const char c) {
  switch (c) {
  case '+':
  case '-':
  case '/':
  case '*':
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
    } else {
      // TODO: error handling
    }
    // default:
    // TODO: error handling
  }
}

std::vector<token> tokenize(const char *s) {
  std::vector<token> tokens{};
  int index = 0;

  while (*s != '\0') {
    if (isDigit(*s)) {
      int num{};
      s += parseNum(s, &num);
      tokens.push_back(token{'\0', num});
    } else if (isUnary(*s)) {
      tokens.push_back(token{*s, 0});
      s++;
    } else if (*s == '(') {
      tokens.push_back(token{*s, 0});
      s++;
    } else if (*s == ')') {
      tokens.push_back(token{*s, 0});
      s++;
    } else {
      s++;
    }
  }

  tokens.push_back(token{0, 0});

  return tokens;
}

int expr(std::vector<token> &tokens, int prec, int index) {
  int rhs{};

  int lhs = tokens[index].val;
  index++;
  char op = tokens[index].type;

  while (isUnary(tokens[index].type) && getPrec(tokens[index].type) >= prec) {
    index++;
    rhs = equation(tokens, getPrec(tokens[index].type) + 1, index);
    lhs = oper(op, lhs, rhs);
  }

  return lhs;
}

int equation(std::vector<token> &tokens, int prec, int index) {
  int result = 0;

  int lhs = tokens[index].val;
  index++;
  char op = tokens[index].type;

  if (tokens[index].type == '(') {
    index++;
    for (; index < tokens.size() && tokens[index].type != ')'; ++index) {
      result = expr(tokens, prec, index);
    }
    index++;
  }

  return result;
}

int main() {
  std::vector<token> tokens = tokenize("(3+  10) * 4");

  printf("%d", equation(tokens, 0, 0));

  return 0;
}