#define __declspec 

#include <emscripten.h>

#include "lcd.h"

EMSCRIPTEN_KEEPALIVE
int numPixelsX(LCD *lcd)
{
  int x = 0;
  if (lcd)
  {
	  numPixels(lcd, &x, 0);
  }
  return x;
}

EMSCRIPTEN_KEEPALIVE
int numPixelsY(LCD *lcd)
{
  int y = 0;
  if (lcd)
  {
	  numPixels(lcd, 0, &y);
  }
  return y;
}