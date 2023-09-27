#pragma once
#include <stdint.h>

#ifndef _KERNEL_SRC

namespace RND
{
    extern __uint128_t g_lehmer64_state;

    uint64_t lehmer64()__attribute__((optimize(0))) ;

    double RandomDouble()__attribute__((optimize(0))) ;

    uint64_t RandomInt()__attribute__((optimize(0))) ;

    void RandomInit(uint64_t a, uint64_t b) __attribute__((optimize(0)));
};

#else

#include "../../kernel/rnd/rnd.h"

#endif