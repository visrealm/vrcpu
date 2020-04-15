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

#ifndef _SIMLIB_COMPUTER_H_
#define _SIMLIB_COMPUTER_H_

#include "simlib.h"
#include "counter.h"
#include "ram.h"
#include "rom.h"
#include "alu.h"
#include "vrEmuLcd.h"

#define uint32_t unsigned

#define BW_PC  ((uint32_t)7)
#define BW_MEM ((uint32_t)6)
#define BW_StP ((uint32_t)5)
#define BW_Ra  ((uint32_t)4)
#define BW_Rb  ((uint32_t)3)
#define BW_Rc  ((uint32_t)2)
#define BW_Rd  ((uint32_t)1)
#define BW_ALU ((uint32_t)0)


#define ALB  ((uint32_t)1 << 6) // B or 00000000 - This isn't inverted..
#define ALC  ((uint32_t)1 << 7) // Carry in

#define _ALW ((uint32_t)1 << 8) // Accept result into ALU register from ALU
#define _RdW ((uint32_t)1 << 9)
#define _RcW ((uint32_t)1 << 10)
#define _RbW ((uint32_t)1 << 11)
#define _RaW ((uint32_t)1 << 12)
#define _StPW ((uint32_t)1 << 13)
#define _MW  ((uint32_t)1 << 14) // Write to memory
#define PGM  ((uint32_t)1 << 15) // Program memory enable

#define _IRW ((uint32_t)1 << 16)
#define PCC  ((uint32_t)1 << 17) // Program counter - increment
#define _PCW ((uint32_t)1 << 18)
#define _MAW ((uint32_t)1 << 19)

#define LCD ((uint32_t)1 << 20)
//#define RdS ((uint32_t)1 << 21) //Output in signed mode

#define _TR  ((uint32_t)1 << 22) // Reset microcode counter
#define HLT ((uint32_t)1 << 23) /// Halt

#define LCD_DATA PGM
#define LCD_COMMAND 0


#define ALS(S) ((uint32_t)S << 3)


typedef struct DLLEXPORT
{
	unsigned tick;
	Bus* bus;
	Register* ra;
	Register* rb;
	Register* rc;
	Register* rd;
	Register* sp;
	Register* ir;
	Register* mar;
	Counter* pc;
	Counter* tc;
	Ram* ram;
	Ram* pgm;
	ALU* alu;
  VrEmuLcd *lcd;

	Rom* rom;
	unsigned controlWord;

	Register *writingToBus;
} Computer;

DLLEXPORT Computer* newComputer();
DLLEXPORT void destroyComputer(Computer* r);

DLLEXPORT void loadProgram(Computer* c, const char* hex);
DLLEXPORT void loadRam(Computer* c, const char* data);

DLLEXPORT byte ramByte(Computer* c, int offset);

// state: 1 = high, 0 = low
DLLEXPORT void computerTick(Computer* r, int high);

DLLEXPORT void computerReset(Computer* c);

#endif
