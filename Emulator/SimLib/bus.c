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


#include "bus.h"
#include <stdlib.h>

DLLEXPORT Bus* newBus()
{
	Bus* r = (Bus*)malloc(sizeof(Bus));
	r->value = 0;
	return r;
}

DLLEXPORT void destroyBus(Bus* r)
{
	free(r);
}

DLLEXPORT void setBusValue(Bus* r, byte newValue)
{
	r->value = newValue;
}

DLLEXPORT byte getBusValue(Bus* r)
{
	return r->value;
}
