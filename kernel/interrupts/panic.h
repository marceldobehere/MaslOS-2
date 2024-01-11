#pragma once

void PrintTaskRegisterDump(void* _renderer, void* _frame);

void PrintRegisterDump(void* renderer);

void Panic(const char* panicMessage, const char* var, bool lock);

void Panic(const char* panicMessage, const char* var);

void Panic(const char* panicMessage);

void Panic(const char* panicMessage, bool lock);

bool CheckKernelSpaceAddr(void* addr);
