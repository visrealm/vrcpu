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

#include "counter.h"
#include <stdlib.h>

DLLEXPORT Counter* newCounter(Bus* b, const char* name, int maxValue)
{
	Counter* c = (Counter*)malloc(sizeof(Counter));
	if (c != NULL)
	{
		c->enabled = 1;
		c->bus = b;
		c->r = newRegister(b, name);
		c->maxValue = maxValue;
	}
	return c;
}

DLLEXPORT void destroyCounter(Counter* c)
{
	destroyRegister(c->r);
	free(c);
}

DLLEXPORT int counterCount(Counter* c)
{

	int currentValue = getRegisterValue(c->r);
	
	if (c->enabled == 0)
		return currentValue;

	if (currentValue == c->maxValue)
	{
		counterReset(c);
		currentValue = 0;
	}
	else
	{
		setRegisterValue(c->r, ++currentValue);
	}
	return currentValue;
}

DLLEXPORT int counterReset(Counter* c)
{
	setRegisterValue(c->r, 0);
	return getRegisterValue(c->r);
}
