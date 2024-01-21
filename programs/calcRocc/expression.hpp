#pragma once

#include "common_includes.hpp"

struct token {
  char type;
  double val;
};

void tokenize(List<void *> *, const char *);
double expr(List<void *> *, long, long);
