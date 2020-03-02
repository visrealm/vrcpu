/*
 * Troy's 8-bit computer - Emulator
 *
 * Copyright (c) 2020 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrcpu
 *
 */

#ifndef _SIMLIB_LCD_H_
#define _SIMLIB_LCD_H_

#include "simlib.h"

#define LCD_CMD_CLEAR                  0b00000001
#define LCD_CMD_HOME                   0b00000010

#define LCD_CMD_ENTRY_MODE             0b00000100
#define LCD_CMD_ENTRY_MODE_INCREMENT   0b00000010
#define LCD_CMD_ENTRY_MODE_DECREMENT   0b00000000
#define LCD_CMD_ENTRY_MODE_SHIFT       0b00000001

#define LCD_CMD_DISPLAY                0b00001000
#define LCD_CMD_DISPLAY_ON             0b00000100
#define LCD_CMD_DISPLAY_CURSOR         0b00000010
#define LCD_CMD_DISPLAY_CURSOR_BLINK   0b00000001

#define LCD_CMD_SHIFT                  0b00010000
#define LCD_CMD_SHIFT_CURSOR           0b00000000
#define LCD_CMD_SHIFT_DISPLAY          0b00001000
#define LCD_CMD_SHIFT_LEFT             0b00000000
#define LCD_CMD_SHIFT_RIGHT            0b00000100

#define LCD_CMD_SET_CGRAM_ADDR         0b01000000
#define LCD_CMD_SET_DRAM_ADDR          0b10000000


typedef struct DLLEXPORT
{
	int width;
	int height;

	byte entryModeFlags;
	byte displayFlags;

	char* data;
	char* ptr;
} LCD;

DLLEXPORT LCD* newLCD(int width, int height);
DLLEXPORT void destroyLCD(LCD* lcd);

DLLEXPORT void sendCommand(LCD* lcd, byte data);
DLLEXPORT void writeByte(LCD *lcd, byte data);
DLLEXPORT byte readByte(LCD* lcd);

DLLEXPORT const char* readLine(LCD* lcd, int row);
DLLEXPORT const byte *charBits(byte c);

#endif