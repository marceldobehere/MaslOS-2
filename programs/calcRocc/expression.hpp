#pragma once

#include <libm/list/list_all.h>

struct token {
  char type;
  int val;
};

int expr(List<token> &, int prec, int index);
