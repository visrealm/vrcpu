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


#include "siminst.h"
#include "computer.h"
#include <stdlib.h>

static Computer* _c = NULL;

SIDLLEXPORT void siInitialise()
{
	if (_c == NULL)
	{
		_c = newComputer();
	}
}

SIDLLEXPORT void siDestroy()
{
	if (_c != NULL)
	{
		destroyComputer(_c);
		_c = NULL;
	}
}

SIDLLEXPORT void siLoadProgram(const char* program)
{
	if (_c)
	{
		loadProgram(_c, program);
		computerReset(_c);
	}
}

// set the clock state (1 = high, 0 = low)
SIDLLEXPORT void siSetClock(int high)
{
	if (_c)
	{
		computerTick(_c, high);
	}
}

SIDLLEXPORT void siReset()
{
	if (_c)
	{
		computerReset(_c);
	}
}

SIDLLEXPORT byte siGetValue(SIComponent component)
{
	if (_c == NULL)
	{
		return 0;
	}

	switch (component)
	{
		case Ra:
			return _c->ra->value;

		case Rb:
			return _c->rb->value;

		case Rc:
			return _c->rc->value;

		case Rd:
			return _c->rd->value;

		case SP:
			return _c->sp->value;

		case PC:
			return _c->pc->r->value;

		case IR:
			return _c->ir->value;

		case TR:
			return _c->tc->r->value;

		case MA:
			return _c->mar->value;

		case ME:
			{
				if (_c->controlWord & PGM)
					return _c->pgm->bytes[_c->mar->value];
			}
			return _c->ram->bytes[_c->mar->value];

		case AL:
			return _c->alu->out->value;

		case FL:
			return _c->alu->flags;

		case BU:
			return _c->bus->value;

		default:
			return 0;
	}
}

SIDLLEXPORT unsigned siGetControlWord()
{
	if (_c)
	{
		return _c->controlWord;
	}
	return 0;

}
