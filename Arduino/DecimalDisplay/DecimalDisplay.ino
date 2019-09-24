/*
 * Troy's 8-bit computer - Arduino DecimalDisplay EEPROM programmer
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * This code is licensed under the MIT license
 * 
 * The dicimal display can operate in three modes controlled by the 
 * EEPROM's 8 and 9 address pins.
 * 
 * 9 | 8
 * --+--
 * 0 | 0  Unsigned decimal
 * 0 | 1  Signed decimal
 * 1 | X  Hexadecimal
 * 
 * https://github.com/visrealm/vrcpu
 *
 */

#include "vrEEPROM.h"


#define EEPROM_D0 5 // data LSB

#define SHIFT_DATA 2 // data to shift into shift register
#define SHIFT_CLK 3 // pos-edge clock for shift register
#define DFF_CLK 4 // pos-edge clock for DFF to set after filling shift register

#define EEPROM_READ_EN A0 // active-low EEPROM read enable
#define EEPROM_WRITE_EN 13 // active-low EEPROM write enable


EEPROM eeprom(EEPROM_D0, EEPROM_READ_EN, EEPROM_WRITE_EN, SHIFT_DATA, SHIFT_CLK, DFF_CLK);


#define DP_HOR_TOP (1 << 5)
#define DP_HOR_MID (1 << 4)
#define DP_HOR_BOTTOM (1 << 1)
#define DP_VER_LTOP (1 << 6)
#define DP_VER_LBOTTOM (1 << 0)
#define DP_VER_RTOP (1 << 7)
#define DP_VER_RBOTTOM (1 << 3)
#define DP_POINT (1 << 2)

uint8_t digits[] = {
  /* 0 */ DP_HOR_TOP | DP_HOR_BOTTOM | DP_VER_LTOP | DP_VER_RTOP | DP_VER_LBOTTOM | DP_VER_RBOTTOM,
  /* 1 */ DP_VER_RTOP | DP_VER_RBOTTOM,
  /* 2 */ DP_HOR_TOP | DP_VER_RTOP | DP_HOR_MID | DP_VER_LBOTTOM | DP_HOR_BOTTOM,
  /* 3 */ DP_HOR_TOP | DP_VER_RTOP | DP_HOR_MID | DP_VER_RBOTTOM | DP_HOR_BOTTOM,
  /* 4 */ DP_VER_LTOP | DP_VER_RTOP | DP_HOR_MID | DP_VER_RBOTTOM,
  /* 5 */ DP_HOR_TOP | DP_VER_LTOP | DP_HOR_MID | DP_VER_RBOTTOM | DP_HOR_BOTTOM,
  /* 6 */ DP_HOR_TOP | DP_VER_LTOP | DP_HOR_MID | DP_VER_LBOTTOM | DP_VER_RBOTTOM | DP_HOR_BOTTOM,
  /* 7 */ DP_HOR_TOP | DP_VER_RTOP | DP_VER_RBOTTOM,
  /* 8 */ DP_HOR_TOP | DP_HOR_MID | DP_HOR_BOTTOM | DP_VER_LTOP | DP_VER_RTOP | DP_VER_LBOTTOM | DP_VER_RBOTTOM,
  /* 9 */ DP_HOR_TOP | DP_HOR_MID | DP_HOR_BOTTOM | DP_VER_LTOP | DP_VER_RTOP | DP_VER_RBOTTOM,
  /* A */ DP_HOR_TOP | DP_HOR_MID | DP_VER_LTOP | DP_VER_RTOP | DP_VER_LBOTTOM | DP_VER_RBOTTOM,
  /* b */ DP_HOR_MID | DP_VER_LTOP | DP_VER_LBOTTOM | DP_VER_RBOTTOM | DP_HOR_BOTTOM,
  /* C */ DP_HOR_TOP | DP_VER_LTOP | DP_VER_LBOTTOM | DP_HOR_BOTTOM,
  /* d */ DP_HOR_MID | DP_VER_RTOP | DP_VER_LBOTTOM | DP_VER_RBOTTOM | DP_HOR_BOTTOM,
  /* E */ DP_HOR_TOP | DP_HOR_MID | DP_HOR_BOTTOM | DP_VER_LBOTTOM | DP_VER_LTOP,
  /* F */ DP_HOR_TOP | DP_HOR_MID | DP_VER_LBOTTOM | DP_VER_LTOP,
  /* H */ DP_HOR_MID | DP_VER_LTOP | DP_VER_RTOP | DP_VER_LBOTTOM | DP_VER_RBOTTOM,
};



byte valueForAddress(uint16_t address)
{
  uint8_t number = address & 0xff;
  bool isSigned =  (address & 0x400) != 0;
  bool isHex =  (address & 0x800) != 0;

  // negative
  if (isHex)
  {
    address -= 2048;
    if (isSigned)
    {
      address -= 1024;
    }

    if (address < 256)
    {
      return 0x00;
    }
    else if (address < 512)
    {
      return digits[16];
    }
    else if (address < 768)
    {
      return digits[(address >> 4) & 0x0f];    
    }
    else
    {
      return digits[address & 0x0f];    
    }

    return 0xff;
  }  
  else if (isSigned)
  {
    if ((address & 0x080) != 0)
    {
      number = ~number + 1;    
    }
    else
    {
      isSigned = false;
    }
    address -= 1024;
  }
  
  
  if (address < 256)  // 00 display mode, 00 digit (0 - 255)
  {
    return isSigned ? DP_HOR_MID : 0x00;
  }
  else if (address < 512) // 00 display mode, 01 digit (256 - 511)
  {
    return digits[(number / 100) % 10];
  }
  else if (address < 768) // 00 display mode, 10 digit (512 - 767)
  {
    return digits[(number / 10) % 10];
  }
  else if (address < 1024) // 00 display mode, 11 digit (512 - 767)
  {
    return digits[number % 10];
  }

  return 0x00;
}

void writeDisplayDigits()
{
  for (uint16_t addr = 0; addr < 4096; ++addr)
  {
    eeprom.write(addr, valueForAddress(addr));
    
    if ((addr % 128) == 0) {
      Serial.print(".");
    }
  }
  eeprom.endWrite();
  return;
}



void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.print("Programming EEPROM");
  Serial.flush();

  writeDisplayDigits();


  
  Serial.println(" Done writing");
  Serial.flush();

  eeprom.dump(0, 4096);

  Serial.println("Done reading");
  Serial.flush();
}

void loop()
{
}
