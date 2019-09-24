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


#ifndef _SIMLIB_REGISTER_H_
#define _SIMLIB_REGISTER_H_

#include "simlib.h"
#include "bus.h"

typedef struct DLLEXPORT
{
	char name[20];
	byte value;
	Bus* bus;
	BufferState state;
} Register;

DLLEXPORT Register* newRegister(Bus *b, const char* name);
DLLEXPORT void destroyRegister(Register* r);

DLLEXPORT void setRegisterValue(Register* r, byte newValue);
DLLEXPORT byte getRegisterValue(Register* r);

DLLEXPORT void registerTick(Register* r);

#endif
