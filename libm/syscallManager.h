#pragma once
#include <stdint.h>
#include <stddef.h>
#include "env/env.h"

int getArgC();
char** getArgV();
ENV_DATA* getEnvData();
void* requestNextPage();

void serialPrint(const char* str);
void serialPrintLn(const char* str);

void serialPrintChar(char c);
char serialReadChar();
bool serialCanReadChar();

void globalPrint(const char* str);
void globalPrintLn(const char* str);
void globalPrintChar(char chr);

void globalCls();

void programExit(int code);
void programCrash();
void programWait(int timeMs);
void programYield();
int programSetPriority(int priority);

uint64_t envGetTimeMs();

uint64_t randomUint64();

void launchTestElfUser();
void launchTestElfKernel();

