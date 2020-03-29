/*
 * Troy's 8-bit computer - Arduino microcode
 *
 * Copyright (c) 2019 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrcpu
 *
 */

#pragma once

#ifdef __AVR_ARCH__
  #include <stdint.h>
  #include <WString.h>
  namespace std
  {
    typedef ::String string;
  }
#else
  #include <cstdint>
  #include <string>
#endif

class EepromAddress;

uint32_t getControlWord(const EepromAddress& address, std::string& desc);
