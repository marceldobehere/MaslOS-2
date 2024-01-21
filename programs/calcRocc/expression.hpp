#pragma once

#include <libm/list/list_all.h>

struct token {
  char type;
  int val;
};

void tokenize(List<token *> *, const char *);
int expr(List<token *> *, int, int);
