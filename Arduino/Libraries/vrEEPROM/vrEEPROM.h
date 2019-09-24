/*
 * Troy's 8-bit computer - Arudino EEPROM class
 * 
 * This class allows for page-write to EEPROMs
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * This code is licensed under the MIT license 
 * 
 * https://github.com/visrealm/vrcpu
 *
 */

#ifndef VR_EEPROM_H
#define VR_EEPROM_H

#include <stdint.h>

class EEPROM {
public:
  EEPROM(int dataPin0, int readPin, int writePin, int data595, int clk595, int rClk595);

  // set the page size (defaults to 128 bytes)
  void setPageSize(int pageSizeBytes);

  uint8_t read(uint16_t address);

  void write(uint16_t address, uint8_t value);
  void endWrite();

  void dump(uint16_t startAddress, uint16_t endAddress);

  private:
    void shiftAddress(uint16_t address);
    void setReadMode();
    void setWriteMode();
    void beginPage();
    void endPage();

  private:
    uint16_t m_currentPage;
    bool m_writeMode;

  private:
    int m_dataPin0;
    int m_dataPin7;
    int m_writePin;
    int m_readPin;
    int m_data595;
    int m_clk595;
    int m_rClk595;
    int m_pageSize;
};

#endif
