#pragma once
#include <stdint.h>
#include <stddef.h>
#include "env/env.h"

int getArgC();
char** getArgV();
ENV_DATA* getEnvData();

void serialPrint(const char* str);
void serialPrintLn(const char* str);

void globalPrint(const char* str);
void globalPrintLn(const char* str);

void globalCls();

void exitProgram(int code);