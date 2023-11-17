#pragma once
#include <libm/window/window.h>
#include <libm/gui/guiInstance.h>
#include "maabMem/maabMemHandler.h"
#include <stdint.h>
#include <stddef.h>

Window* FindWindowWithId(uint64_t winID);
GuiInstance* GetGuiFromWindow(Window* win);

enum DatatypeNumber : uint8_t
{
	INT = 0,
	UINT = 1,
	SHORT = 2,
	USHORT = 3,
	LONG = 4,
	ULONG = 5,
	CHAR = 6,
	BOOL = 7,
	FLOAT = 8,
	DOUBLE = 9
};

const uint8_t datatypeSizes[10] =
{ 4,4, 2,2, 8,8, 1,1, 4,8 };

enum OpNumber : uint8_t
{
	PLUS,
	MINUS,
	MULT,
	DIVIDE,
	MOD,
	EQUALS,
	NOT_EQUALS,
	GREATER,
	GREATER_EQUALS,
	LESS,
	LESS_EQUAL,
	BOOL_AND,
	BOOL_OR,
	BOOL_NOT,
	BIT_AND,
	BIT_OR,
	BIT_NOT,
	BITSHIFT_LEFT,
	BITSHIFT_RIGHT

};

void Cast(DatatypeNumber typeFrom, uint64_t addrFrom, DatatypeNumber typeTo, uint64_t addrTo);
void PrintVal(DatatypeNumber numType, uint64_t numAddr);
void ShowBytes(uint8_t amount, uint64_t addr);
void Math(OpNumber opNum, DatatypeNumber typeNum, uint64_t addr1, uint64_t addr2, uint64_t addrRes);


void Do();
void Free();
void PrintMem();

