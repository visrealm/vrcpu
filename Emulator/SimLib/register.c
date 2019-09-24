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

#include "register.h"
#include <stdlib.h>
#include <string.h>

DLLEXPORT Register* newRegister(Bus* b, const char* name)
{
	Register *r = (Register*)malloc(sizeof(Register));
	if (r != NULL)
	{
		r->state = Floating;
		r->value = 0xff;
		r->bus = b;
		strncpy(r->name, name, sizeof(r->name) - 1);
	}
	return r;
}

DLLEXPORT void destroyRegister(Register* r)
{
	free(r);
}

DLLEXPORT void setRegisterValue(Register* r, byte newValue)
{
	r->value = newValue;
}

DLLEXPORT byte getRegisterValue(Register* r)
{
	return r->value;
}

DLLEXPORT void registerTick(Register* r)
{
	switch (r->state)
	{
		case WriteToBus:
			setBusValue(r->bus, getRegisterValue(r));
			break;

		case ReadFromBus:
			setRegisterValue(r, getBusValue(r->bus));
			break;

		default:
			break;
	}
}