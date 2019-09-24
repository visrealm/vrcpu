/*
 * Troy's 8-bit computer - Emulator
 *
 * Copyright (c) 2019 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrcpu
 *
 */

#ifndef _SIMLIB_ROM_H_
#define _SIMLIB_ROM_H_

#include "simlib.h"

typedef struct DLLEXPORT
{
	int size;
	byte* bytes;
} Rom;

DLLEXPORT Rom* newRomFromFile(const char* romFile);
DLLEXPORT Rom* newRomFromString(const char* romStr);
DLLEXPORT void destroyRom(Rom* r);

DLLEXPORT byte readRom(Rom* r, int address);

#endif
