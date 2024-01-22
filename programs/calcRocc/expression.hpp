#pragma once

#include "common_includes.hpp"

struct Token {
  char type;
  double val;
};

void tokenize(List<void *> *, const char *);
double parse(List<void *> *, long, long);
