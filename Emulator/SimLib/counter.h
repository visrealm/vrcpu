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

#ifndef _SIMLIB_COUNTER_H_
#define _SIMLIB_COUNTER_H_

#include "simlib.h"
#include "register.h"

typedef struct DLLEXPORT
{
	int enabled;
	int maxValue;
	Register* r;
	Bus* bus;
} Counter;

DLLEXPORT Counter* newCounter(Bus* b, const char* name, int maxValue);
DLLEXPORT void destroyCounter(Counter* r);

DLLEXPORT int counterCount(Counter* c);
DLLEXPORT int counterReset(Counter* c);

#endif
