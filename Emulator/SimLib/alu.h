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

#ifndef _SIMLIB_ALU_H_
#define _SIMLIB_ALU_H_

#include "simlib.h"
#include "register.h"


#define B_MINUS_A 0b001
#define A_MINUS_B 0b010
#define A_PLUS_B  0b011
#define A_XOR_B   0b100
#define A_OR_B    0b101
#define A_AND_B   0b110


#define INC_A     0b000
#define NOT_A     0b111

#define ALU_ALL   0b111


#define FLAG_NEG   0b0001
#define FLAG_OFLOW 0b0010
#define FLAG_CARRY 0b0100
#define FLAG_ZERO  0b1000


typedef struct DLLEXPORT
{
	byte mode;
	int useRb;
	int carryIn;

	byte flags;

	Register* out;
	Register* rb;
} ALU;

DLLEXPORT ALU* newALU(Bus* b, Register *rb);
DLLEXPORT void destroyALU(ALU* r);

DLLEXPORT void aluTick(ALU* a);

#endif
