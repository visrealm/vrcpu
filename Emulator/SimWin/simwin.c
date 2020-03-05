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
#include "lcd.h"
#include <stdio.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void printReg(const char *name, byte value)
{
	printf("%s = "BYTE_TO_BINARY_PATTERN"\n", name, BYTE_TO_BINARY(value));
}

/*void printMem(Ram* ram)
{
	printf("%s = "BYTE_TO_BINARY_PATTERN", "BYTE_TO_BINARY_PATTERN"\n", ram->value->name, BYTE_TO_BINARY(ram->bytes[0]), BYTE_TO_BINARY(ram->bytes[1]));
}*/


void printLine(const char *line)
{
	for (int y = 0; y < 8; ++y)
	{
		char* c = line;
		while ((*c) != '\0')
		{
			byte* bits = charBits(*c);
			for (int x = 0; x < 5; ++x)
			{
				printf("%c", bits[x] & (0x80 >> y) ? 'X' : ' ');
			}
			printf(" ");
			++c;
		}
		printf("\n");
	}

}

void outputLcd(LCD* lcd)
{
	int width = 0, height = 0;
	numPixels(lcd, &width, &height);
	updatePixels(lcd);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			char pix = pixelState(lcd, x, y);
			printf("%c", (pix < 0) ? ' ' : (pix ? 'X' : '.'));
		}
		printf("\n");
	}
}

int main()
{
	LCD* lcd = newLCD(5, 2);
	sendCommand(lcd, LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_INCREMENT);
	writeByte(lcd, 'H');
	writeByte(lcd, 'e');
	writeByte(lcd, 'l');
	writeByte(lcd, 'l');
	writeByte(lcd, 'o');
	writeByte(lcd, ',');
	writeByte(lcd, ' ');
	writeByte(lcd, 'w');
	writeByte(lcd, 'o');
	writeByte(lcd, 'r');
	writeByte(lcd, 'l');
	writeByte(lcd, 'd');
	writeByte(lcd, '!');
	sendCommand(lcd, LCD_CMD_SET_DRAM_ADDR | (7));
	writeString(lcd, "Troy_");

	outputLcd(lcd);

	//printf("%s\n", readLine(lcd, 0));

//	printLine(readLine(lcd, 0));
//	printLine(readLine(lcd, 1));
}

#if 0
int main()
{
	siInitialise();

	// fib
	siLoadProgram("37c1ce38001a110b2f02");

	// bounce
	// siLoadProgram("3718c03e01e018e03e062f00");

	// isprime
	// siLoadProgram("1f390be13e082f1213ca3e0e192d3c032f091f012d");

	// powers
	//siLoadProgram("000701b8000f01b9011713b54f01fd39002f094700c00f0fd8392fb80018081700e0382a1a0a47002f1fce38332f210f00b9016e");

	byte lastD = -1;
	int tick = 0;
	while (1)
	{
		siSetClock((++tick % 2) == 0);
		if (siGetControlWord() & HLT)
			break;

		if (siGetValue(TR) != 0) continue;
	/*
		printf("Control word: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
			BYTE_TO_BINARY((c->controlWord & 0xff0000) >> 16), BYTE_TO_BINARY((c->controlWord & 0xff00) >> 8), BYTE_TO_BINARY((c->controlWord & 0xff)));
		printReg(c->ra);
		printReg(c->rb);
		printReg(c->rc);
		printReg(c->rd);
		printReg(c->pc->r);
		printReg(c->ir);
		printReg(c->sp);
		printReg(c->alu->out);
		printReg(c->tc->r);
		printMem(c->ram);
		getchar();
		printf("\n\n");
		*/
		int j = 9;
		for (int i = 0; i < 100000000; ++i)
		{
			j += i;
		}
		if (lastD != siGetValue(Rd))
		{
			lastD = siGetValue(Rd);
			printf("\r%d   ", lastD);
			
		
		//	getchar();
		}
	}


	siDestroy();

	return 0;
}
#endif