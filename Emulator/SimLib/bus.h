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

#ifndef _SIMLIB_BUS_H_
#define _SIMLIB_BUS_H_

#include "simlib.h"

typedef struct DLLEXPORT
{
	byte value;
} Bus;

typedef enum 
{
	Floating,
	WriteToBus,
	ReadFromBus,
} BufferState;

DLLEXPORT Bus* newBus();
DLLEXPORT void destroyBus(Bus* r);

DLLEXPORT void setBusValue(Bus* r, byte newValue);
DLLEXPORT byte getBusValue(Bus* r);

#endif
