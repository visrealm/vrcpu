#define __declspec 

#include <emscripten.h>
#include "siminst.h"

EMSCRIPTEN_KEEPALIVE 
void simLibInitialise()
{
	siInitialise();
}

EMSCRIPTEN_KEEPALIVE
void simLibDestroy()
{
	siDestroy();
}

EMSCRIPTEN_KEEPALIVE
void simLibLoadProgram(const char* program)
{
	siLoadProgram(program);
}

// set the clock state (1 = high, 0 = low)
EMSCRIPTEN_KEEPALIVE
void simLibSetClock(int high)
{
	siSetClock(high);
}

EMSCRIPTEN_KEEPALIVE
void simLibReset()
{
	siReset();
}

EMSCRIPTEN_KEEPALIVE
int simLibGetValue(SIComponent component)
{
	return (int)siGetValue(component);
}

EMSCRIPTEN_KEEPALIVE
int simLibGetControlWord()
{
	return (int)siGetControlWord();
}


EMSCRIPTEN_KEEPALIVE
void simLibLcdCommand(int rs, unsigned char data)
{
	siLcdCommand(rs, data);
}

EMSCRIPTEN_KEEPALIVE
int simLibLcdNumPixelsX()
{
	return (int)siLcdNumPixelsX();
}

EMSCRIPTEN_KEEPALIVE
int simLibLcdNumPixelsY()
{
	return (int)siLcdNumPixelsY();
}

EMSCRIPTEN_KEEPALIVE
int simLibLcdPixelState(int x, int y)
{
	return (int)siLcdPixelState(x, y);
}
