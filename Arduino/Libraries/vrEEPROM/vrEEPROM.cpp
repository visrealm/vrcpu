/*
 * Troy's 8-bit computer - Arudino EEPROM class
 * 
 * This class allows for page-write to EEPROMs (Tested on Greenliant GLS29EE010)
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * This code is licensed under the MIT license 
 * 
 * https://github.com/visrealm/vrcpu
 *
 */

#include "vrEEPROM.h"
#include "Arduino.h"

#define DEFAULT_PAGE_SIZE 128


EEPROM::EEPROM(
  int dataPin0,
  int readPin,
  int writePin,
  int data595,
  int clk595,
  int rClk595) : m_currentPage(-1),
                m_writeMode(false),
                m_dataPin0(dataPin0),
                m_dataPin7(dataPin0 + 7),
                m_readPin(readPin),
                m_writePin(writePin),
                m_data595(data595),
                m_clk595(clk595),
                m_rClk595(rClk595),
                m_pageSize(DEFAULT_PAGE_SIZE)
{
  setReadMode();
}


void EEPROM::setPageSize(int pageSizeBytes)
{
  m_pageSize = pageSizeBytes;
}

uint8_t EEPROM::read(uint16_t address)
{
  if (m_writeMode)
  {
    endPage();
    setReadMode();
  }
    // Set up the address
  shiftAddress(address);
  digitalWrite(m_rClk595, HIGH);
  digitalWrite(m_rClk595, LOW);
  delayMicroseconds(1);
  // Perform the read
  byte data = 0;
  for (int pin = m_dataPin7; pin >= m_dataPin0; --pin) // for each data pin in reverse
  {
    data = (data << 1) + digitalRead(pin);
  }

  return data;
}

void EEPROM::write(uint16_t address, uint8_t value)
{
  if (!m_writeMode)
  {
    setWriteMode();
  }

  if (m_currentPage != -2)
  {
    uint16_t currentPage = address / m_pageSize;
    if (currentPage != m_currentPage)
    {
      if (m_currentPage != -1)
      {
        endPage();
      }
      m_currentPage = -1;
      beginPage();
    }
    m_currentPage = currentPage;
  }
  
  // Set up the address
  shiftAddress(address);

  // End the previous write if there was one
  digitalWrite(m_writePin, HIGH);
  delayMicroseconds(1);

  // Show the new address to the EEPROM
  digitalWrite(m_rClk595, HIGH);
  digitalWrite(m_rClk595, LOW);
  
  delayMicroseconds(1);

  // Set up the data
  for (int pin = m_dataPin0; pin <= m_dataPin7; ++pin) // for each data pin
  {
    digitalWrite(pin, value & 1);
    value = value >> 1;
  }
  delayMicroseconds(1);

  // Start the write
  digitalWrite(m_writePin, LOW);
//  digitalWrite(m_writePin, HIGH);
}

void EEPROM::endWrite()
{
    endPage();
}

void EEPROM::dump(uint16_t startAddress, uint16_t endAddress)
{
  unsigned long baseAddr;

  byte data[16];
  for (baseAddr = startAddress; baseAddr < endAddress; baseAddr += 16UL) // for every 16 addresses in the EEPROM
  {
    for (unsigned int offset = 0U; offset < 16U; offset++) // for each address within the current set of 16 addresses
    {
      data[offset] = read(baseAddr + offset);
    }

    char buf[80];
    sprintf(buf, "%05lx: %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
      baseAddr, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10],
      data[11], data[12], data[13], data[14], data[15]);
    // the %05lx above has an L, not a one

    Serial.println(buf);
    Serial.flush();
  }
}

void EEPROM::shiftAddress(uint16_t address)
{
  shiftOut(m_data595, m_clk595, LSBFIRST, (address));       // Outputs XXXX XXXX (bits 0-7)
  shiftOut(m_data595, m_clk595, LSBFIRST, (address >> 8));  // Outputs XXXX XXXX (bits 8-15)
  //shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, (address >> 16)); // Outputs 0000 000X (bits 16-23)
}

void EEPROM::setReadMode()
{
  for (int pin = m_dataPin0; pin <= m_dataPin7; pin++) // for each data pin
  {
    pinMode(pin, INPUT);
  }

  pinMode(m_data595, OUTPUT);
  digitalWrite(m_data595, LOW);
  pinMode(m_clk595, OUTPUT);
  digitalWrite(m_clk595, LOW);
  pinMode(m_rClk595, OUTPUT);
  digitalWrite(m_rClk595, LOW);

  pinMode(m_readPin, OUTPUT);
  digitalWrite(m_readPin, LOW); // always read
  pinMode(m_writePin, OUTPUT);
  digitalWrite(m_writePin, HIGH);

  m_writeMode = false;
}

void EEPROM::setWriteMode()
{
  pinMode(m_data595, OUTPUT);
  digitalWrite(m_data595, LOW);
  pinMode(m_clk595, OUTPUT);
  digitalWrite(m_clk595, LOW);
  pinMode(m_rClk595, OUTPUT);
  digitalWrite(m_rClk595, LOW);

  pinMode(m_readPin, OUTPUT);
  digitalWrite(m_readPin, HIGH);
  pinMode(m_writePin, OUTPUT);
  digitalWrite(m_writePin, HIGH);

  for (int pin = m_dataPin0; pin <= m_dataPin7; pin++) // for each data pin
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
 
  m_writeMode = true;
}

void EEPROM::beginPage()
{
  // TODO: Need a way to only trigger this when not doing this.
  m_currentPage = -2;
  write(0x5555, 0xAA);
  write(0x2AAA, 0x55);
  write(0x5555, 0xA0);
  
}
void EEPROM::endPage()
{
   if (m_writeMode && m_currentPage != -1)
   {
      digitalWrite(m_writePin, HIGH);
      delay(30);
      m_currentPage = -1;
   }
}
