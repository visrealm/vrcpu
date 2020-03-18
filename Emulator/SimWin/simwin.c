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

#include <windows.h>
#undef byte
#include "siminst.h"
#include "computer.h"
#include "lcd.h"
#include <stdio.h>
#include <conio.h>

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


void outputLcd(LCD* lcd)
{
	static int hasOutput = 0;
	if (0 && hasOutput)
	{
		printf("%c[18A", 27);
	}


	hasOutput = 1;

	int width = 0, height = 0;
	numPixels(lcd, &width, &height);
	updatePixels(lcd);

	char* line = malloc(width + 1);
	if (line)
	{
		line[width] = '\0';

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				char pix = pixelState(lcd, x, y);
				line[x] = (pix < 0) ? ' ' : (pix ? '#' : '.');
			}
			printf("%s\n", line);
		}

		free(line);
	}
}
int main(int argc, char* argv[])
{
  char* portName = NULL;

  for (int i = 1; i < argc; ++i)
  {
    if (strncmp(argv[i], "-port", 6) == 0)
    {
      ++i;
      if (i < argc)
      {
        portName = argv[i];
      }
    }
  }

  if (portName == NULL)
  {
    printf("Usage: simwin -port <PORTNAME>\n");
    return 0;
  }


  HANDLE hComm;  // Handle to the Serial port
  BOOL   Status; // Status
  DCB dcbSerialParams = { 0 };  // Initializing DCB structure
  COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
  char SerialBuffer[100] = { 0 }; //Buffer to send and receive data
  DWORD BytesWritten = 0;          // No of bytes written to the port
  DWORD dwEventMask;     // Event mask to trigger
  char  ReadData;        //temperory Character
  DWORD NoBytesRead;     // Bytes read by ReadFile()
  unsigned char loop = 0;
  wchar_t PortNo[40] = { 0 }; //contain friendly name
  LCD* lcd = NULL;

  swprintf_s(PortNo, 40, L"%s", portName);
  //Open the serial com port
  hComm = CreateFile(PortNo, //friendly name
    GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
    0,                                 // No Sharing, ports cant be shared
    NULL,                              // No Security
    OPEN_EXISTING,                     // Open existing port only
    0,                                 // Non Overlapped I/O
    NULL);                             // Null for Comm Devices
  if (hComm == INVALID_HANDLE_VALUE)
  {
    printf_s("\nPort %s can't be opened\n\n", portName);
    goto Exit2;
  }
  //Setting the Parameters for the SerialPort
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  Status = GetCommState(hComm, &dcbSerialParams); //retreives  the current settings
  if (Status == FALSE)
  {
    printf_s("\nError to Get the Com state\n\n");
    goto Exit1;
  }
  dcbSerialParams.BaudRate = CBR_115200;      //BaudRate = 9600
  dcbSerialParams.ByteSize = 8;             //ByteSize = 8
  dcbSerialParams.StopBits = ONESTOPBIT;    //StopBits = 1
  dcbSerialParams.Parity = NOPARITY;      //Parity = None
  Status = SetCommState(hComm, &dcbSerialParams);
  if (Status == FALSE)
  {
    printf_s("\nError to Setting DCB Structure\n\n");
    goto Exit1;
  }
  //Setting Timeouts
  timeouts.ReadIntervalTimeout = 50;
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;
  if (SetCommTimeouts(hComm, &timeouts) == FALSE)
  {
    printf_s("\nError to Setting Time outs");
    goto Exit1;
  }

  lcd = newLCD(16, 2);
  sendCommand(lcd, LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON);// | LCD_CMD_DISPLAY_CURSOR_BLINK);
  //sendCommand(lcd, LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_SHIFT | LCD_CMD_ENTRY_MODE_INCREMENT);// | LCD_CMD_ENTRY_MODE_DECREMENT);

  while (1)
  {
    char mode = 0;
    unsigned int data = 0;

    mode = 0;
    data = 0;
    printf_s("Mode (d/i/s/q): ");
    scanf_s("%c", &mode);

    if (mode == 'q')
    {
      break;
    }
    else if (mode == 'i')
    {
      printf_s("Data (hex): ");
      scanf_s("%x", &data);
      snprintf(SerialBuffer, sizeof(SerialBuffer), "%c %02x", mode, data);

      sendCommand(lcd, data);
    }
    else if (mode == 'd')
    {
      printf_s("Data (hex): ");
      scanf_s("%x", &data);
      snprintf(SerialBuffer, sizeof(SerialBuffer), "%c %02x", mode, data);

      writeByte(lcd, data);
    }
    else if (mode == 's')
    {
      char buffer[80];
      printf_s("Text: ");
      gets(buffer);
      gets(buffer);
      snprintf(SerialBuffer, sizeof(SerialBuffer), "%c %s", mode, buffer);

      writeString(lcd, buffer);
    }
    else
    {
      printf("Invalid option: %c\n"
        " i: instruction mode\n"
        " d: data mode\n"
        " s: send string\n"
        " q: quit\n", mode);
      continue;
    }

    outputLcd(lcd);

    printf("%s\n", SerialBuffer);

    //Writing data to Serial Port
    Status = WriteFile(hComm,// Handle to the Serialport
      SerialBuffer,            // Data to be written to the port
      strlen(SerialBuffer) + 1,   // No of bytes to write into the port
      &BytesWritten,  // No of bytes written to the port
      NULL);
    //print numbers of byte written to the serial port
    printf_s("\nNumber of bytes written to the serial port = %d\n\n", BytesWritten);///BytesWritten);

    if (Status == FALSE)
    {
      printf_s("\nFailed to write");
      goto Exit1;
    }
  }
Exit1:
  CloseHandle(hComm);//Closing the Serial Port
  if (lcd != NULL) destroyLCD(lcd);
Exit2:
  system("pause");
  return 0;
}

int main2()
{
	LCD* lcd = newLCD(16, 2);
	sendCommand(lcd, LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON | LCD_CMD_DISPLAY_CURSOR);
	//sendCommand(lcd, LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_SHIFT | LCD_CMD_ENTRY_MODE_INCREMENT);// | LCD_CMD_ENTRY_MODE_DECREMENT);
	//sendCommand(lcd, LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_RIGHT);
	//sendCommand(lcd, LCD_CMD_SET_DRAM_ADDR | (1));
	sendCommand(lcd, LCD_CMD_SET_CGRAM_ADDR);
	writeByte(lcd, 0x1f);
	writeByte(lcd, 0x11);
	writeByte(lcd, 0x11);
	writeByte(lcd, 0x11);
	writeByte(lcd, 0x11);
	writeByte(lcd, 0x11);
	writeByte(lcd, 0x11);
	writeByte(lcd, 0x1f);

	writeByte(lcd, 0x04);
	writeByte(lcd, 0x0e);
	writeByte(lcd, 0x04);
	writeByte(lcd, 0x1f);
	writeByte(lcd, 0x04);
	writeByte(lcd, 0x04);
	writeByte(lcd, 0x0a);
	writeByte(lcd, 0x11);

	sendCommand(lcd, LCD_CMD_SET_DRAM_ADDR);


	char c = 0;
	while (c != 27)
	{
		if (c)
		{
			if (c == -32)
			{
				c = _getch() % 128;
				switch (c)
				{
					case 'K': // left
						sendCommand(lcd, LCD_CMD_SHIFT | LCD_CMD_SHIFT_CURSOR | LCD_CMD_SHIFT_LEFT);
						break;

					case 'M': // right
						sendCommand(lcd, LCD_CMD_SHIFT | LCD_CMD_SHIFT_CURSOR | LCD_CMD_SHIFT_RIGHT);
						break;

					case 'H': // up
						sendCommand(lcd, LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_LEFT);
						break;

					case 'P': // down
						sendCommand(lcd, LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_RIGHT);
						break;

					case 5: // F11
						writeByte(lcd, 0);
						break;

					case 6: // F12
						writeByte(lcd, 1);
						break;

					default:
						writeByte(lcd, c);
						break;
				}
			}
			else
			{
				writeByte(lcd, c);
			}
		}
		outputLcd(lcd);
		c = _getch();
	}


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