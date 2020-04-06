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

#include "alu.h"
#include <stdlib.h>

DLLEXPORT ALU* newALU(Bus *b, Register* rb)
{
	ALU* a = (ALU*)malloc(sizeof(ALU));
	if (a != NULL)
	{
		a->rb = rb;
		a->carryIn = 0;
		a->useRb = 0;
		a->flags = 0;
		a->out = newRegister(b, "ALU");
		a->mode = INC_A;
	}
	return a;
}

DLLEXPORT void destroyALU(ALU* a)
{
	destroyRegister(a->out);
	free(a);
}


DLLEXPORT void aluTick(ALU* a)
{
	if (a->out->state == ReadFromBus)
	{
		byte valA = a->out->bus->value;

		byte carry = a->carryIn;
		int isMinus = 0;
		if (a->mode == A_MINUS_B || a->mode == B_MINUS_A)
		{
			isMinus = 1;
			carry = 1 - a->carryIn;
		}

		byte valB = a->useRb ? getRegisterValue(a->rb) : carry;
		byte valOut = 0;
		unsigned valOut2 = 0;
		int oflow = 0;

		switch (a->mode)
		{
			case B_MINUS_A:
				valOut = valB - (valA + (a->useRb ? carry : 0));
				valOut2 = valB - (valA + (a->useRb ? carry : 0));
				break;

			case A_MINUS_B:
				valOut = valA - (valB + (a->useRb ? carry : 0));
				valOut2 = valA - (valB + (a->useRb ? carry : 0));
				break;

			case A_PLUS_B:
				valOut = valA + valB + (a->useRb ? carry : 0);
				valOut2 = valA + valB + (a->useRb ? carry : 0);
				break;

			case A_XOR_B:
				valOut = valA ^ valB;
				valOut2 = valA ^ valB;
				oflow = carry;
				break;

			case A_OR_B:
				valOut = valA | valB;
				valOut2 = valA | valB;
				oflow = carry;
				break;

			case A_AND_B:
				valOut = valA & valB;
				valOut2 = valA & valB;
				oflow = carry;
				break;

			default:
				break;
		}

		byte truncatedVal = valOut & 0xff;

		unsigned carryFlag = isMinus ? 0 : FLAG_CARRY;
		unsigned notCarryFlag = isMinus ? FLAG_CARRY : 0;

		a->flags = ((truncatedVal & 0x80) ? FLAG_NEG : 0) |
				   ((truncatedVal == 0) ? FLAG_ZERO : 0) |
				   (((((valOut2 & 0xff)) != valOut2)) ? carryFlag : notCarryFlag) |
				   (oflow ? FLAG_OFLOW : 0);

		if (a->flags & FLAG_ZERO)
		{
			int jj = 0;
		}

		setRegisterValue(a->out, truncatedVal);
	}
}
