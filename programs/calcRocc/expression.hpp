#pragma once

#include "common_includes.hpp"

struct token {
  char type;
  int val;
};

void tokenize(List<token *> *, const char *);
double expr(List<token *> *, long, long);
