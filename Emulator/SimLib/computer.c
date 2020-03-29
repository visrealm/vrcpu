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


#include "computer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

DLLEXPORT Computer* newComputer()
{
	Computer* c = (Computer*)malloc(sizeof(Computer));
	if (c != NULL)
	{
		c->tick = 0;
		c->bus = newBus();
		c->ra = newRegister(c->bus, "Ra");
		c->rb = newRegister(c->bus, "Rb");
		c->rc = newRegister(c->bus, "Rc");
		c->rd = newRegister(c->bus, "Rd");
		c->sp = newRegister(c->bus, "SP");
		c->ir = newRegister(c->bus, "IR");
		c->mar = newRegister(c->bus, "MAR");
		c->pc = newCounter(c->bus, "PC", 255);
		c->pc->enabled = 0;
		c->controlWord = 0;

		c->tc = newCounter(c->bus, "Step", 0x07);

		c->ram = newRam(c->bus, c->mar, 256);
		c->pgm = newRam(c->bus, c->mar, 256);

    c->alu = newALU(c->bus, c->rb);
    c->lcd = vrEmuLcdNew(16, 2, EmuLcdRomA00);
    c->rom= newRomFromFile("rom.hex");

		c->writingToBus = NULL;
	}
	return c;
}

DLLEXPORT void destroyComputer(Computer* c)
{
	destroyRegister(c->ra);
	destroyRegister(c->rb);
	destroyRegister(c->rc);
	destroyRegister(c->rd);
	destroyRegister(c->sp);
	destroyRegister(c->ir);
	destroyCounter(c->pc);
	destroyCounter(c->tc);
	destroyRam(c->ram);
	destroyRam(c->pgm);
	destroyRegister(c->mar);
  destroyALU(c->alu);
  vrEmuLcdDestroy(c->lcd);
  destroyBus(c->bus);
	memset(c, sizeof(Computer), 0);
	free(c);
}


void setWriting(Computer* c, Register* r)
{
	if (c->writingToBus != NULL && c->writingToBus->state == WriteToBus)
	{
		c->writingToBus->state = Floating;
	}
	c->writingToBus = r;
	r->state = WriteToBus;
}

DLLEXPORT void loadProgram(Computer* c, const char* hex)
{
	size_t bytes = strlen(hex) / 2;
	for (int i = 0; i < bytes; ++i)
	{
		byte b = 0;
		sscanf(hex + i * 2, "%02hhX", &b);
		writeRam(c->pgm, i, b);
	}
}


DLLEXPORT void computerTick(Computer* c, int high)
{
	if (c->controlWord & HLT)
		return;

	if (high == 0)
	{
		unsigned romAddr = c->ir->value | (c->tc->r->value << 8) | (c->alu->flags << 11);
		counterCount(c->tc);
		counterCount(c->pc);

		c->controlWord = *((unsigned*) & (c->rom->bytes[romAddr * 4]));
		//printf("%u: %u\n", romAddr, c->controlWord);

		c->alu->out->state = (c->controlWord & _ALW) ? Floating : ReadFromBus;
		c->rd->state = (c->controlWord & _RdW) ? Floating : ReadFromBus;
		c->rc->state = (c->controlWord & _RcW) ? Floating : ReadFromBus;
		c->rb->state = (c->controlWord & _RbW) ? Floating : ReadFromBus;
		c->ra->state = (c->controlWord & _RaW) ? Floating : ReadFromBus;
		c->sp->state = (c->controlWord & _StPW) ? Floating : ReadFromBus;
		c->ram->value->state = (c->controlWord & _MW) ? Floating : ((c->controlWord & PGM) ? Floating : ReadFromBus);
		c->pgm->value->state = (c->controlWord & _MW) ? Floating : ((c->controlWord & PGM) ? ReadFromBus : Floating);
		c->ir->state = (c->controlWord & _IRW) ? Floating : ReadFromBus;
		c->pc->r->state = (c->controlWord & _PCW) ? Floating : ReadFromBus;
		c->mar->state = (c->controlWord & _MAW) ? Floating : ReadFromBus;

		if ((c->controlWord & _TR) == 0)
		{
			counterReset(c->tc);
		}

		c->pc->enabled = (c->controlWord & PCC) ? 1 : 0;



		switch (c->controlWord & 0x7)
		{
		case BW_PC:
			setWriting(c, c->pc->r);
			break;

		case BW_MEM:
			if ((c->controlWord & PGM) != 0)
			{
				setWriting(c, c->pgm->value);
				ramTick(c->pgm);
			}
			else
			{
				setWriting(c, c->ram->value);
				ramTick(c->ram);
			}
			break;

		case BW_StP:
			setWriting(c, c->sp);
			break;

		case BW_Ra:
			setWriting(c, c->ra);
			break;

		case BW_Rb:
			setWriting(c, c->rb);
			break;

		case BW_Rc:
			setWriting(c, c->rc);
			break;

		case BW_Rd:
			setWriting(c, c->rd);
			break;

		case BW_ALU:
			setWriting(c, c->alu->out);
			break;
		}

		c->alu->carryIn = (c->controlWord & ALC) ? 1 : 0;
		c->alu->useRb = (c->controlWord & ALB) ? 1 : 0;
		c->alu->mode = (c->controlWord & ALS(ALU_ALL)) >> 3;
	}
	else
	{
		if (c->writingToBus)
		{
			registerTick(c->writingToBus);
		}

		aluTick(c->alu);


		registerTick(c->ra);
		registerTick(c->rb);
		registerTick(c->rc);
		registerTick(c->rd);
		registerTick(c->ir);
		registerTick(c->sp);
		registerTick(c->ra);
		registerTick(c->mar);
		registerTick(c->pc->r);

		if (c->ram->value->state == ReadFromBus)
			ramTick(c->ram);
		if (c->pgm->value->state == ReadFromBus)
			ramTick(c->pgm);

    if (c->controlWord & LCD)
    {
      if (c->controlWord & PGM)
      {
        vrEmuLcdSendCommand(c->lcd, c->bus->value);
      }
      else
      {
        vrEmuLcdWriteByte(c->lcd, c->bus->value);
      }
    }
	}

	++c->tick;
}


DLLEXPORT void computerReset(Computer* c)
{
	counterReset(c->pc);
	counterReset(c->tc);
	c->controlWord = 0;
}
