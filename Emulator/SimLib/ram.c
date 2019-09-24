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


#include "ram.h"
#include <stdlib.h>

DLLEXPORT Ram* newRam(Bus* b, Register* mar, int size)
{
	Ram* r = (Ram*)malloc(sizeof(Ram));
	if (r != NULL)
	{
		srand((unsigned)r);
		r->mar = mar;
		r->value = newRegister(b, "Mem");
		r->size = size;
		r->bytes = malloc(size);
		for (int i = 0; i < size; ++i)
		{
			r->bytes[i] = rand() & 0xff;
		}
	}
	return r;
}

DLLEXPORT void destroyRam(Ram* r)
{
	destroyRegister(r->value);
	free(r->bytes);
	free(r);
}

DLLEXPORT byte readRam(Ram* r, int address)
{
	if (address >= 0 && address < r->size)
	{
		return r->bytes[address];
	}
	return 0;
}

DLLEXPORT void writeRam(Ram* r, int address, byte value)
{
	if (address >= 0 && address < r->size)
	{
		r->bytes[address] = value;
	}
}

DLLEXPORT void ramTick(Ram* r)
{
	switch (r->value->state)
	{
		case WriteToBus:
			setRegisterValue(r->value, readRam(r, getRegisterValue(r->mar)));
			setBusValue(r->value->bus, getRegisterValue(r->value));
			break;

		case ReadFromBus:
			writeRam(r, r->mar->value, getBusValue(r->value->bus));
			break;

		default:
			break;
	}
}