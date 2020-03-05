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


#include "rom.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

DLLEXPORT Rom* newRomFromFile(const char *romFile)
{
	Rom* r = (Rom*)malloc(sizeof(Rom));
	if (r != NULL)
	{
		byte buf[10];
		r->size = 32767 * 4;
		FILE* f = NULL;
		f = fopen(romFile, "r");
		if (f == NULL)
		{
			printf("Unable to load ROM file: %s\n", romFile);
			exit(1);
		}
		else
		{
			r->bytes = malloc(r->size);
			memset(r->bytes, 0, r->size);

			for (int i = 0; i < r->size; i += 4)
			{
				fgets(buf, 3, f);
				sscanf(buf, "%02hhX", r->bytes + i + 3);
				fgets(buf, 3, f);
				sscanf(buf, "%02hhX", r->bytes + i + 2);
				fgets(buf, 3, f);
				sscanf(buf, "%02hhX", r->bytes + i + 1);
				fgets(buf, 3, f);
				sscanf(buf, "%02hhX", r->bytes + i);
			}

			fclose(f);
		}
	}
	return r;
}

DLLEXPORT Rom* newRomFromString(const char* romStr)
{
	Rom* r = (Rom*)malloc(sizeof(Rom));
	if (r != NULL)
	{
		byte buf[10] = {0};
		r->size = 32767 * 4;
		r->bytes = malloc(r->size);
		memset(r->bytes, 0, r->size);

		for (int i = 0; i < r->size; i += 4)
		{
			sscanf(buf, "%08x", (unsigned*)(romStr + i));
		}
	}
	return r;
}

DLLEXPORT void destroyRom(Rom* r)
{
	free(r->bytes);
	free(r);
}

DLLEXPORT byte readRom(Rom* r, int address)
{
	if (address >= 0 && address < r->size)
	{
		return r->bytes[address];
	}
	return 0;
}
