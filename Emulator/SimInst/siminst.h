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

#ifndef _SIMINST_H_
#define _SIMINST_H_

#if _EMSCRIPTEN
#define SIDLLEXPORT
#elif SI_COMPILING_DLL
#define SIDLLEXPORT __declspec(dllexport)
#else
#define SIDLLEXPORT __declspec(dllimport)
#endif

#define byte unsigned char

#include "vrEmuLcd.h"

typedef enum SIDLLEXPORT
{
	Ra = 0,
	Rb = 1,
	Rc = 2,
	Rd = 3,
	SP = 4,
	PC = 5,
	IR = 6,
	TR = 7,
	MA = 8,
	ME = 9,
	AL = 10,
	FL = 11,
	BU = 12,
} SIComponent;


SIDLLEXPORT void siInitialise();
SIDLLEXPORT void siDestroy();

SIDLLEXPORT void siLoadProgram(const char* program);

SIDLLEXPORT void siLoadRam(const char* data);

SIDLLEXPORT byte siRamByte(int offset);

SIDLLEXPORT void siSetInput(byte inputByte);

// set the clock state (1 = high, 0 = low)
SIDLLEXPORT void siSetClock(int high);

SIDLLEXPORT void siReset();

SIDLLEXPORT byte siGetValue(SIComponent component);

SIDLLEXPORT VrEmuLcd* siGetLcd();

SIDLLEXPORT unsigned siGetControlWord();


#endif