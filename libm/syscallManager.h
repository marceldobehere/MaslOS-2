#pragma once
#include <stdint.h>
#include <stddef.h>
#include "env/env.h"

int getArgC();
char** getArgV();
ENV_DATA* getEnvData();

