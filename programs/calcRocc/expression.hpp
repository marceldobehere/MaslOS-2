#pragma once

#include "common_includes.hpp"

struct Token
{
    char type;
    double val;
};

bool tokenize(List<void *> *, const char *);
bool parse(List<void *> *, long, long, double *);
