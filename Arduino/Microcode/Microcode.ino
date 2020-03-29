/*
 * Troy's 8-bit computer - Arduino microcode EEPROM programmer
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * Based on https://github.com/jamesbates/jcpu
 * 
 * This code is licensed under the MIT license
 * 
 * This EEPROM write is page-write enabled. It writes all data sequentially.
 * It also only writes changed pages. This can be much faster than writing
 * the entire EEPROM for minor changes.
 * 
 * All writes are verified with a read. In the  serial monitor:
 * 
 * . = unchanged page
 * o = page written successfully
 * X = page write failed
 * 
 * https://github.com/visrealm/vrcpu
 *
 */

#include "vrEEPROM.h"
#include "Constants.h"
#include "Microcode.h"

#define EEPROM_D0 5 // data LSB

#define SHIFT_DATA 2 // data to shift into shift register
#define SHIFT_CLK 3 // pos-edge clock for shift register
#define DFF_CLK 4 // pos-edge clock for DFF to set after filling shift register

#define EEPROM_READ_EN A0 // active-low EEPROM read enable
#define EEPROM_WRITE_EN 13 // active-low EEPROM write enable


 // Required for page-write enabled EEPROMs
 // What is the page size?
#define PAGE_SIZE 128
#define TOTAL_BYTES 1 << 15
#define NUM_PAGES (TOTAL_BYTES / PAGE_SIZE)

int ROM_NO = -1;
EEPROM eeprom(EEPROM_D0, EEPROM_READ_EN, EEPROM_WRITE_EN, SHIFT_DATA, SHIFT_CLK, DFF_CLK);

void setup()
{
  Serial.begin(115200);
  eeprom.setPageSize(PAGE_SIZE);
}

bool validatePage(int page)
{
  uint16_t pageStart = page * PAGE_SIZE;

  String desc;

  for (uint16_t offset = 0; offset < PAGE_SIZE; ++offset)
  {
    EepromAddress addr(pageStart + offset);
    uint32_t controlWord = flipActiveLows(getControlWord(addr, desc));
    uint8_t romByte = (controlWord >> ROM_NO * 8) & 0xff;

    if (eeprom.read(addr) != romByte) 
    {
      return false;
    }
  }
  return true;
}

// the setup function runs once when you press reset or power the board
void loop() {
  Serial.println("Troy's EEPROM programmer. ");

  Serial.println("Choose ROM to program (2, 1 or 0) ... ('d' to dump)");
  
  ROM_NO = -1;
  
  while (ROM_NO < 0)
  {
    if (Serial.available() > 0)
    {
      switch (Serial.read())
      {
        case '0':
          ROM_NO = 0;
          break;

        case '1':
          ROM_NO = 1;
          break;

        case '2':
          ROM_NO = 2;
          break;

        case 'd':
        case 'D':
          Serial.println("");
          eeprom.dump(0, 0x7FFF);
          return;
          break;

        default:
          break;
      }
    }
  }

  Serial.print("Programming EEPROM #");
  Serial.println(ROM_NO);

  Serial.print("Pages: ");
  Serial.println(NUM_PAGES);

  for (int currentPage = 0; currentPage < NUM_PAGES; ++currentPage)
  {
    if (currentPage > 0 && (currentPage % 64) == 0)
    {
      Serial.println();
    }
    
    uint16_t pageStart = currentPage * PAGE_SIZE;

    // check page
    if (validatePage(currentPage))
    {
      Serial.print('.');
    }
    else
    {
      // write page
      for (uint16_t offset = 0; offset < PAGE_SIZE; ++offset)
      {
        String desc;
        EepromAddress addr(pageStart + offset);
        uint32_t controlWord = flipActiveLows(getControlWord(addr, desc));
        uint8_t romByte = (controlWord >> (ROM_NO * 8)) & 0xff;
  
        eeprom.write(addr, romByte);
      }
      eeprom.endWrite();

      if (validatePage(currentPage))
      {
        Serial.print('o');
      }
      else
      {
        Serial.print("X");
      }
    }
  }

  Serial.println();
 
  Serial.print("Done programming EEPROM #");
  Serial.println(ROM_NO);
}
