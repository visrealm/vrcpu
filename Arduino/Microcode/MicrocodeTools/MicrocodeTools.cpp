/*
 * Troy's 8-bit computer - Arduino microcode generator for emulator
 *
 * Copyright (c) 2019 Troy Schrapel
 *
 * Based on https://github.com/jamesbates/jcpu
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrcpu
 *
 */

#include <iostream>
#include <fstream>
#include "Microcode.h"
#include "Constants.h"


int main()
{
  std::ofstream romFile("cpemu.data", std::ios::trunc);

  char buf[10];
  for (int address = 0; address < EepromAddress::TOTAL_BYTES; ++address)
  {
    EepromAddress addr(address);

    std::string desc = "<Unassigned>";

    uint32_t controlWord = flipActiveLows(getControlWord(addr, desc));
    snprintf(buf, sizeof(buf), "%08x", controlWord);
    romFile << buf;

    if (addr.flags() == 0 && addr.microtime() == 2)
    {
      printf("%03d: %s: %s\n", (int)addr.opcode(), addr.opcode().bitsToString().c_str(), desc.c_str());
    }
  }
}
