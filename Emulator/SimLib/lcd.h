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

struct LCD_s;
typedef struct LCD_s LCD;

DLLEXPORT LCD* newLCD(int width, int height);
DLLEXPORT void destroyLCD(LCD* lcd);

DLLEXPORT void sendCommand(LCD* lcd, byte data);
DLLEXPORT void writeByte(LCD* lcd, byte data);
DLLEXPORT void writeString(LCD* lcd, const char *str);
DLLEXPORT int getDataOffset(LCD* lcd, int row, int col);
DLLEXPORT byte readByte(LCD* lcd);

DLLEXPORT const char* readLine(LCD* lcd, int row);
DLLEXPORT const byte *charBits(LCD* lcd, byte c);

DLLEXPORT void updatePixels(LCD* lcd);
DLLEXPORT void numPixels(LCD *lcd, int* width, int* height);
DLLEXPORT char pixelState(LCD *lcd, int x, int y);

#endif