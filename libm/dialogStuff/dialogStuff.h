#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Dialog
{
    const char* OpenFileDialog();
    const char* OpenFileDialog(const char* path);
};