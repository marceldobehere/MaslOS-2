#pragma once
#include <stdint.h>

extern bool PrintAll;

void PercentDone(int percent);
void StepDone();
void StepDone(int step);
void PrepBootScreen();

void PrintSpaces();
//void PrintMsgColSL(const char* msg, const char* var, uint32_t col);
void Println();

//void PrintMsgCol(const char* msg, const char* var, uint32_t col);
void PrintMsgStartLayer(const char* msg);
void PrintMsgEndLayer(const char* msg);

void PrintMsg(const char* msg);
//void PrintMsg(const char* msg, const char* var);
void PrintMsgCol(const char* msg, uint32_t col);
void PrintfMsg(const char* msg, ...);
void PrintfMsgCol(const char* msg, uint32_t col, ...);

void PrintMsgSL(const char* msg);
//void PrintMsgSL(const char* msg, const char* var);
void PrintMsgColSL(const char* msg, uint32_t col);
void PrintfMsgSL(const char* msg, ...);
void PrintfMsgColSL(const char* msg, uint32_t col, ...);


void PrintDebugTerminal();

