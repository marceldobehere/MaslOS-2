#pragma once
#include <stdint.h>
#include <stddef.h>

void _memcpy(void* src, void* dest, uint64_t size);
void _memset(void* dest, uint8_t value, uint64_t size);
