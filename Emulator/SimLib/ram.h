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


#ifndef _SIMLIB_RAM_H_
#define _SIMLIB_RAM_H_

#include "simlib.h"
#include "register.h"

typedef struct DLLEXPORT
{
	Register* mar;
	Register* value;
	int size;
	byte* bytes;
} Ram;

DLLEXPORT Ram* newRam(Bus *b, Register *mar, int size);
DLLEXPORT void destroyRam(Ram* r);

DLLEXPORT byte readRam(Ram* r, int address);
DLLEXPORT void writeRam(Ram* r, int address, byte value);

DLLEXPORT void ramTick(Ram* r);

#endif
