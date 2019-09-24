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

#include "EEPROM.h"


// Output a report of unused op codes after each EEPROM is written ?
#define REPORT_UNUSED   1

// Output the full rom image as hex ascii. This is used for the emulator.
// Note: When this option is enabled, Nothing is displayed in the serial
//       monitor. Just send '1' and it will output the image. 
//       This image contains the contents of all ROMS in one
#define OUTPUT_ROM_FILE 1

// Output additional debugging info?
//#define VERBOSE 1


// Required for page-write enabled EEPROMs
// What is the page size?
#define PAGE_SIZE 128
#define TOTAL_BYTES 32767
#define NUM_PAGES (TOTAL_BYTES / PAGE_SIZE)

// Dump the contents to serial monitor
//#define DUMP 1

#define EEPROM_D0 5 // data LSB

#define SHIFT_DATA 2 // data to shift into shift register
#define SHIFT_CLK 3 // pos-edge clock for shift register
#define DFF_CLK 4 // pos-edge clock for DFF to set after filling shift register

#define EEPROM_READ_EN A0 // active-low EEPROM read enable
#define EEPROM_WRITE_EN 13 // active-low EEPROM write enable

template <size_t BITS>
class Bitset
{
  public:
    Bitset()
    {
      clear();
    }

    void clear(bool value = false)
    {
      memset(m_bytes, value ? 0xffffffff : 0x00, bytes());
    }

    void set(size_t b, bool value)
    {
      size_t by = byteIndex(b);
      if (by != (size_t)-1)
      {
        if (value)
          bitSet(m_bytes[by], bitIndex(b));
        else
          bitClear(m_bytes[by], bitIndex(b));
      }      
    }

    bool isSet(size_t b) const
    {
      size_t by = byteIndex(b);
      if (by != (size_t)-1)
      {
        return bitRead(m_bytes[by], bitIndex(b));
      }
      return false;
    }

    size_t bytes() const { return sizeof(m_bytes); }

  private:
    size_t byteIndex(size_t b) const
    {
      if (b < BITS)
      {
        return b / 8;
      }
      return (size_t)-1;
    }

    size_t bitIndex(size_t b) const
    {
      return b % 8;
    }

  private:
    byte m_bytes[(BITS >> 3) + 1];  
  
};



/* signal word: bit position  31     30     29     28     27     26     25     24     23     22     21     20     19     18     17     16                 
 *              meaning       HLT    -      -      -      -      -      BW_MEM    _MW    HLT   PGM    _MAW   _IRW   _RdE   _RdW   BW_Rc   _RcW
 *              bit position  15     14     13     12     11     10     09     08     07     06     05     04     03     02     01     00
 *              meaning       _RbE   _RbW   _RaE   _RaW   BW_PC   _PCW   BW_StP   _StPW   PCC    BW_ALU   _ALW   ALB   ALS2   ALS1   ALS0   ALC
 */

#define BW_PC  ((uint32_t)7)
#define BW_MEM ((uint32_t)6)
#define BW_StP ((uint32_t)5)
#define BW_Ra  ((uint32_t)4)
#define BW_Rb  ((uint32_t)3)
#define BW_Rc  ((uint32_t)2)
#define BW_Rd  ((uint32_t)1)
#define BW_ALU ((uint32_t)0)


#define ALB  ((uint32_t)1 << 6) // B or 00000000 - This isn't inverted..
#define ALC  ((uint32_t)1 << 7) // Carry in

#define _ALW ((uint32_t)1 << 8) // Accept result into ALU register from ALU
#define _RdW ((uint32_t)1 << 9)
#define _RcW ((uint32_t)1 << 10)
#define _RbW ((uint32_t)1 << 11)
#define _RaW ((uint32_t)1 << 12)
#define _StPW ((uint32_t)1 << 13)
#define _MW  ((uint32_t)1 << 14) // Write to memory
#define PGM  ((uint32_t)1 << 15) // Program memory enable

#define _IRW ((uint32_t)1 << 16)
#define PCC  ((uint32_t)1 << 17) // Program counter - increment
#define _PCW ((uint32_t)1 << 18)
#define _MAW ((uint32_t)1 << 19)

// possible flip-flop??
//#define RdH ((uint32_t)1 << 20) //Output in hex mode
//#define RdS ((uint32_t)1 << 21) //Output in signed mode

#define _TR  ((uint32_t)1 << 22) // Reset microcode counter
#define HLT ((uint32_t)1 << 23) /// Halt

#define ALS(S) ((uint32_t)S << 3)

#define B_MINUS_A 0b001
#define A_MINUS_B 0b010
#define A_PLUS_B  0b011
#define A_XOR_B   0b100
#define A_OR_B    0b101
#define A_AND_B   0b110


#define INC_A     0b000
#define NOT_A     0b111


/* EEPROM address:  bit position  14        13         12       11         10  ....   8      7   .....    0
 *                  meaning       zero      carry   oflow      neg        (microtime) T              opcode
 */


#define NEG_ADDR   ((uint32_t)1 << 11)
#define OFLOW_ADDR ((uint32_t)1 << 12)
#define CARRY_ADDR ((uint32_t)1 << 13)
#define ZERO_ADDR  ((uint32_t)1 << 14)

#define FLAG_MASK ((uint32_t)15 << 11)

#define ins_eeprom_address(opcode, flags, T) ((opcode << 0) | (flags) | (T << 8))


#define MOV 0b00
#define LOD 0b01
#define STO 0b10


#define Ra  0b000
#define Rb  0b001
#define Rc  0b010
#define Rd  0b011
#define StP  0b100
#define PC  0b101
#define StPi 0b110  // StackPointer OR ALU
#define Acc 0b110   // StackPointer OR ALU
#define IMM 0b111


#define OPCODE(op,dreg,sreg) (((op & 0b11) << 6) | ((dreg & 0b111) << 3) | (sreg & 0b111))
#define ALU_OPCODE(with_carry,S,reg) ((0b11 << 6) | (with_carry ? 1 << 5 : 0) | ((S & 0b111) << 2) | (reg & 0b11))




int ROM_NO = -1;



template<class T>
void print(const T &v)
{
#ifndef OUTPUT_ROM_FILE
  Serial.print(v);
#endif
}

template<class T>
void println(const T &v)
{
#ifndef OUTPUT_ROM_FILE
  Serial.println(v);
#endif
}

template<class T>
void print(const T &v, int f)
{
#ifndef OUTPUT_ROM_FILE
  Serial.print(v, f);
#endif
}

template<class T>
void println(const T &v, int f)
{
#ifndef OUTPUT_ROM_FILE
  Serial.println(v, f);
#endif
}

uint32_t inline flip_active_lows(uint32_t microcode_word) {

  microcode_word ^= (_ALW | _StPW | _PCW | _RaW | _RbW | _RcW | _RdW | _MAW | _IRW | _MW | _TR);
  return microcode_word;
}

uint32_t _E(uint32_t reg) {

  switch (reg) {
    case Ra: return BW_Ra;
    case Rb: return BW_Rb;
    case Rc: return BW_Rc;
    case Rd: return BW_Rd;
    case StP: return BW_StP;
    case PC: return BW_PC;
    default: return 0; 
  }
}

uint32_t _W(uint8_t reg) {

  switch (reg) {
    case Ra: return _RaW;
    case Rb: return _RbW;
    case Rc: return _RcW;
    case Rd: return _RdW;
    case StP: return _StPW;
    case PC: return _PCW;
    default: return 0;
  }
}

#define FETCH0 (BW_PC | _MAW)
#define FETCH1 (PGM | BW_MEM | _IRW | PCC)

uint32_t microcode[8] = {FETCH0,FETCH1,_TR,0,0,0,0,0};


uint32_t *MICROCODE0() {
  microcode[2] = _TR;
  microcode[3] = microcode[4] = microcode[5] = microcode[6] = microcode[7] = 0;
  return microcode;
}

uint32_t *MICROCODE1(uint32_t c1) {
  microcode[2] = c1 | _TR;
  microcode[3] = microcode[4] = microcode[5] = microcode[6] = microcode[7] = 0;
  return microcode;
}

uint32_t *MICROCODE2(uint32_t c1, uint32_t c2) {
  microcode[2] = c1;
  microcode[3] = c2 | _TR;
  microcode[4] = microcode[5] = microcode[6] = microcode[7] = 0;
  return microcode;
}

uint32_t *MICROCODE3(uint32_t c1,uint32_t c2,uint32_t c3) {
  microcode[2] = c1;
  microcode[3] = c2;
  microcode[4] = c3 | _TR;
  microcode[5] = microcode[6] = microcode[7] = 0;
  return microcode;
}

uint32_t *MICROCODE4(uint32_t c1,uint32_t c2,uint32_t c3, uint32_t c4) {
  microcode[2] = c1;
  microcode[3] = c2;
  microcode[4] = c3;
  microcode[5] = c4 | _TR;
  microcode[6] = microcode[7] = 0;
  return microcode;
}

uint32_t *MICROCODE5(uint32_t c1,uint32_t c2,uint32_t c3, uint32_t c4, uint32_t c5) {
  microcode[2] = c1;
  microcode[3] = c2;
  microcode[4] = c3;
  microcode[5] = c4;
  microcode[6] = c5 | _TR;
  microcode[7] = 0;
  return microcode;
}

uint32_t *MICROCODE6(uint32_t c1,uint32_t c2,uint32_t c3, uint32_t c4, uint32_t c5, uint32_t c6) {
  microcode[2] = c1;
  microcode[3] = c2;
  microcode[4] = c3;
  microcode[5] = c4;
  microcode[6] = c5;
  microcode[7] = c6 | _TR;
  return microcode;
}


EEPROM eeprom(EEPROM_D0, EEPROM_READ_EN, EEPROM_WRITE_EN, SHIFT_DATA, SHIFT_CLK, DFF_CLK);


void writeConditionalInstruction(uint16_t opcode, uint32_t flags, uint32_t microcode[]) {

  flags &= FLAG_MASK;
  
  for (uint8_t T = 0; T < 8; T++) {
    uint16_t eeprom_address = ins_eeprom_address(opcode, flags, T);
    uint32_t full_data = flip_active_lows(microcode[T]);
#ifdef OUTPUT_ROM_FILE
    uint32_t eeprom_byte =  full_data;
#else
    uint8_t eeprom_byte =  (full_data >> (8 * ROM_NO)) & 0xFF;
#endif
    if (false)//(opcode & 0xff) == OPCODE(MOV, IMM, (1<<0)))
    {
      print(eeprom_address, BIN);
      print(": ");
      println(full_data, BIN);
    }
    doWrite(eeprom_address, eeprom_byte);
  }
}

void writeFlagCondInstruction(uint16_t opcode, uint32_t flag, uint32_t microcode[]) {

  flag &= FLAG_MASK;

  for (uint32_t f = 0; f <= 0b1111; ++f)
  {
    uint32_t actualFlag = f << 11;
    if ((flag & actualFlag) != 0)
    {    
      writeConditionalInstruction(opcode, actualFlag, microcode);
    }
  }
}

void writeNoFlagCondInstruction(uint16_t opcode, uint32_t flag, uint32_t microcode[]) {

  flag &= FLAG_MASK;
  
  for (uint32_t f = 0; f <= 0b1111; ++f)
  {
    uint32_t actualFlag = f << 11;
    if ((flag & actualFlag) == 0)
    {    
      writeConditionalInstruction(opcode, actualFlag, microcode);
    }
  }
}

void writeCarrycondInstruction(uint16_t opcode, bool carry, uint32_t microcode[])
{
  if (carry)
  {
    writeFlagCondInstruction(opcode, CARRY_ADDR, microcode);
  }
  else
  {
    writeNoFlagCondInstruction(opcode, CARRY_ADDR, microcode);
  }
}


void writeInstruction(uint16_t opcode, uint32_t microcode[]) {

  writeNoFlagCondInstruction(opcode, 0, microcode);
}

void serialOutputBegin(String ins)
{
  return;
  print("Writing ");
  print(ins);
  print(" instructions: ");
}

void serialOutputStep()
{
  return;
  print(".");
}

void serialOutputDone()
{
  return;
  println(". done.");
}

void serialOutputAddress(uint16_t address)
{
  print("Opcode: ");
  print(address & 0xff, BIN);
  print("    Flags: ");
  print((address & ZERO_ADDR) ? "Z" : "");
  print((address & CARRY_ADDR) ? "C" : "");
  print((address & NEG_ADDR) ? "N" : "");
  print((address & OFLOW_ADDR) ? "O" : "");
  print("    Step: ");
  println((address & 0x700) >> 8);

}

int currentPage = 0;
#ifdef OUTPUT_ROM_FILE
uint32_t pageBytes[PAGE_SIZE] = {0};
#else
byte pageBytes[PAGE_SIZE] = {0};
#endif
bool pageDirty = false;

Bitset<NUM_PAGES> pageValid;
Bitset<256> opcodeSet;


bool isCheckingStage()
{
  return currentPage == -1;
}


void doWrite(uint16_t address, uint8_t value)
{
  int pageByteIndex = address % PAGE_SIZE;
    
  if (isCheckingStage() ||
      (address >= (currentPage * PAGE_SIZE) && address < ((currentPage + 1) * PAGE_SIZE)))
  {
    pageBytes[pageByteIndex] = value;

    int pageNum = address / PAGE_SIZE;
    if (pageNum >= NUM_PAGES)
      return;
    
    if (isCheckingStage())
    {
      opcodeSet.set(address & 0xff, true);
      if (pageValid.isSet(pageNum))
      {
        pageValid.set(pageNum, verify(address));
      }
      else
      {
        verify(address);
      }
    }
    else
    {
      int index = address - (currentPage * PAGE_SIZE);
      
      if (opcodeSet.isSet(index))
      {
        Serial.print("Setting ");
        Serial.print(address & 0xff, BIN);
        Serial.println(" twice?");
      }
      opcodeSet.set(index, true);            
    }
  }
}


void doWrite(uint16_t address, uint32_t value)
{
  int pageByteIndex = address % PAGE_SIZE;
    
  if ((address >= (currentPage * PAGE_SIZE) && address < ((currentPage + 1) * PAGE_SIZE)))
  {
    pageBytes[pageByteIndex] = value;
  }
}

bool verify(uint16_t address)
{
  byte value = pageBytes[address % PAGE_SIZE];
  byte currentData = eeprom.read(address);
  if (currentData != value)
  {
#ifdef VERBOSE
    print(address, HEX);
    print(" i: ");
    print(address % PAGE_SIZE);
    print("  ");
    serialOutputAddress(address);
    print("  Current: ");
    println(currentData, BIN);
    print("  New: ");
    println(value, BIN);
#endif
    return false;
  }
  return true;
}

#ifdef INS_MOV 
void write_MOVs() {

  serialOutputBegin("reg <- reg MOV");  // 00 <dst> <src>
  for (uint8_t sreg = Ra; sreg <= PC; sreg++) {

    for (uint8_t dreg = Ra; dreg <= Acc; dreg++) {
    serialOutputStep();

      // jmz
      if (sreg == PC && dreg == Acc)
      {
        writeInstruction(OPCODE(MOV, Acc, sreg), MICROCODE2(_E(PC) | ALS(A_AND_B) | _ALW, _W(PC)));
      }
      // normal move
      else if (sreg != dreg) {        
        writeInstruction(OPCODE(MOV, dreg, sreg), MICROCODE1(_W(dreg) | _E(sreg)));
      }
      // move to accumulator
      else if (dreg == Acc)
      {
        // MOV to Acc
        writeInstruction(OPCODE(MOV, Acc, sreg), MICROCODE1(_E(sreg) | ALS(A_PLUS_B) | _ALW));
      }
    }
  }
  serialOutputDone();

  serialOutputBegin("reg <- imm DATA"); // 00 <dst> 111  <nextbyte>
  for (uint8_t dreg = Ra; dreg <= PC; dreg++) {
    serialOutputStep();

    writeInstruction(OPCODE(MOV, dreg, IMM), MICROCODE2(BW_PC | _MAW , (dreg == PC ? 0 : PCC) | PGM | BW_MEM | _W(dreg)));
  }
  writeInstruction(OPCODE(MOV, StPi, IMM), MICROCODE2(_E(PC) | ALS(A_AND_B) | _ALW, _W(Ra) | _W(Rb) | _W(Rc) | _W(Rd) | BW_ALU));
  serialOutputDone();

  serialOutputBegin("NOP and HLT"); // NOP: 00 000 000   HLT: 00 101 101
  writeInstruction(OPCODE(MOV, Ra, Ra), MICROCODE0());
  writeInstruction(OPCODE(MOV, PC, PC), MICROCODE1(HLT));
  serialOutputDone();
  
  serialOutputBegin("conditional JC, JZ, JO and JN");   // JC: 00111000 JZ: 00111001 JO: 00111010 JN: 00111100
  writeCarrycondInstruction(OPCODE(MOV, IMM, 0b000), false, MICROCODE1(PCC));
  writeCarrycondInstruction(OPCODE(MOV, IMM, 0b000), true, MICROCODE2(_MAW | BW_PC, PGM | BW_MEM | _PCW));
  serialOutputStep();
  
  for (uint8_t flag = 0; flag <= 2; flag++) {
    uint32_t flagBits = 0;
    switch (flag)
    {
      case 0:
        flagBits = ZERO_ADDR;
        break;
        
      case 1:
        flagBits = OFLOW_ADDR;
        break;
        
      case 2:
        flagBits = NEG_ADDR;
        break;
        
      default:
        break;
    }

    
    writeNoFlagCondInstruction(OPCODE(MOV, IMM, 1<<flag), flagBits, MICROCODE1(PCC));
    writeFlagCondInstruction(OPCODE(MOV, IMM, 1<<flag), flagBits, MICROCODE2(_MAW | BW_PC , PGM | BW_MEM | _PCW));
    serialOutputStep();
  }
  serialOutputDone();

  serialOutputBegin("conditional JNC, JNZ, JNO and JNN");  // JNC: 00111111 JNZ: 00111110 JNO: 00111101 JNN: 00111011
  writeCarrycondInstruction(OPCODE(MOV, IMM, 0b111), true, MICROCODE1(PCC));
  writeCarrycondInstruction(OPCODE(MOV, IMM, 0b111), false, MICROCODE2(_MAW | BW_PC , PGM | BW_MEM | _PCW));
  serialOutputStep();
  
  for (uint8_t flag = 0; flag <= 2; flag++) {
    uint32_t flagBits = 0;
    switch (flag)
    {
      case 0:
        flagBits = ZERO_ADDR;
        break;
        
      case 1:
        flagBits = OFLOW_ADDR;
        break;
        
      case 2:
        flagBits = NEG_ADDR;
        break;
        
      default:
        break;
    }

    
    writeFlagCondInstruction(OPCODE(MOV, IMM, ~(1<<flag)), flagBits, MICROCODE1(PCC));
    writeNoFlagCondInstruction(OPCODE(MOV, IMM, ~(1<<flag)), flagBits, MICROCODE2(_MAW | BW_PC, PGM | BW_MEM | _PCW));
    serialOutputStep();
  }
  serialOutputDone();
}
#endif

#ifdef INS_LOD
void write_LODs() {

  serialOutputBegin("reg <- [reg] LOD");
  for (uint8_t sreg = Ra; sreg <= PC; sreg++) {

    for (uint8_t dreg = Ra; dreg <= PC; dreg++) {
    serialOutputStep();

      writeInstruction(OPCODE(LOD, dreg, sreg), MICROCODE2(_MAW | _E(sreg), ((sreg == Rc) ? PGM : 0) | BW_MEM | _W(dreg)));
    }
  }
  serialOutputDone();

  serialOutputBegin("reg <- [StPi] POP");
  for (uint8_t dreg = Ra; dreg < PC; dreg++) {

    serialOutputStep();
    // pop
    writeInstruction(OPCODE(LOD, dreg, StPi), MICROCODE3(BW_StP | _ALW | ALC | ALS(A_PLUS_B) | _MAW,  _StPW | BW_ALU, _W(dreg) | BW_MEM)); 

    // peek
    writeInstruction(OPCODE(LOD, StPi, dreg), MICROCODE2(BW_StP | _MAW, _W(dreg) | BW_MEM)); 
  }
  // pop imm (trashes Rc)
  writeInstruction(OPCODE(LOD, IMM, StPi), MICROCODE5(BW_StP | _ALW | ALC | ALS(A_PLUS_B) | _MAW,  _StPW | BW_ALU, _W(Rc) | BW_MEM, _MAW | BW_PC, PCC | _MW | _E(Rc))); 
  // peek imm (trashes Rc)
  writeInstruction(OPCODE(LOD, StPi, IMM), MICROCODE4(BW_StP | _MAW,  _W(Rc) | BW_MEM, _MAW | BW_PC, PCC | _MW | _E(Rc))); 
  
  serialOutputDone();


  serialOutputBegin("ret");
  serialOutputStep();
  writeInstruction(OPCODE(LOD, PC, StPi), MICROCODE3(BW_StP | _ALW | ALC | ALS(A_PLUS_B) | _MAW,  _StPW | BW_ALU, _W(PC) | BW_MEM));
  serialOutputDone();


  serialOutputBegin("reg <- [IMM] LOD");
  for (uint8_t dreg = Ra; dreg <= PC; dreg++) 
  {
    serialOutputStep();

    writeInstruction(OPCODE(LOD, dreg, IMM), MICROCODE3(_MAW | BW_PC , PCC | BW_MEM | PGM | _MAW, BW_MEM | _W(dreg))); 
  }  
  serialOutputDone();

  serialOutputBegin("unused LOD");

}
#endif

#ifdef INS_STO
void write_STOs() {

  serialOutputBegin("[reg] <- reg STO");
  for (uint8_t sreg = Ra; sreg <= PC; sreg++) {

    for (uint8_t dreg = Ra; dreg <= PC; dreg++) {
    serialOutputStep();

      writeInstruction(OPCODE(STO, dreg, sreg), MICROCODE2(_MAW | _E(dreg), _MW | _E(sreg)));
    }
  }
  serialOutputDone();

  serialOutputBegin("[StPi] <- reg PUSH.");
  for (uint8_t sreg = Ra; sreg <= StP; sreg++) {

    serialOutputStep();
    writeInstruction(OPCODE(STO, StPi, sreg), MICROCODE3(BW_StP | _ALW | ALS(A_MINUS_B), _StPW | BW_ALU | _MAW, _E(sreg) | _MW)); 
  }
  // push immediate (trashes Rc) push the value directly
  writeInstruction(OPCODE(STO, StPi, IMM), MICROCODE5(BW_StP | _ALW | ALS(A_MINUS_B), _StPW | BW_ALU | _MAW, BW_MEM | _W(Rc), _MAW | BW_PC, PCC | _E(Rc) | _MW)); 
  serialOutputDone();

  serialOutputBegin("CALL");
  writeInstruction(OPCODE(STO, StPi, PC), MICROCODE4(BW_StP | _ALW | ALS(A_MINUS_B), _StPW | BW_ALU | _MAW, BW_PC | _MW, _PCW | BW_Rc));
  serialOutputDone();

  serialOutputBegin("CALL IMMEDIATE");
  writeInstruction(OPCODE(STO, IMM, PC), MICROCODE6(BW_PC | _MAW, PCC | PGM | BW_MEM | _W(Rc), BW_StP | _ALW | ALS(A_MINUS_B), _StPW | BW_ALU | _MAW, BW_PC | _MW, _PCW | BW_Rc));
  serialOutputDone();

  serialOutputBegin("[IMM] <- reg STO");
  for (uint8_t sreg = Ra; sreg < PC; sreg++) {

    serialOutputStep();
    writeInstruction(OPCODE(STO, IMM, sreg), MICROCODE3(_MAW | BW_PC, BW_MEM | PCC | PGM | _MAW, _MW | _E(sreg)));
  }
  serialOutputDone();
}


void write_ALU_instructions() {

  serialOutputBegin("INC reg <- reg + 1 and DEC reg <- reg - 1");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, INC_A, reg), MICROCODE2(_E(reg) | ALS(A_PLUS_B) | ALC | _ALW, _W(reg) | BW_ALU));
  }
  serialOutputStep();

  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(true, INC_A, reg), MICROCODE2(_E(reg) | ALS(A_MINUS_B) | _ALW, _W(reg) | BW_ALU));
  }
  serialOutputDone();

  serialOutputBegin("ADD|ADC reg <- reg + Rb");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, A_PLUS_B, reg),                 MICROCODE2(_E(reg) | ALB | ALS(A_PLUS_B) | _ALW, _W(reg) | BW_ALU));
    writeCarrycondInstruction(ALU_OPCODE(true, A_PLUS_B, reg), false, MICROCODE2(_E(reg) | ALB | ALS(A_PLUS_B) | _ALW, _W(reg) | BW_ALU));
    writeCarrycondInstruction(ALU_OPCODE(true, A_PLUS_B, reg), true,  MICROCODE2(_E(reg) | ALB | ALS(A_PLUS_B) | _ALW | ALC, _W(reg) | BW_ALU));    
  }
  serialOutputDone();

  serialOutputBegin("SUB|SBC reg <- reg - Rb");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, A_MINUS_B, reg),  MICROCODE2(_E(reg) | ALB | ALC | ALS(A_MINUS_B) | _ALW, _W(reg) | BW_ALU));
    writeCarrycondInstruction(ALU_OPCODE(true, A_MINUS_B, reg), false, MICROCODE2(_E(reg) | ALB | ALS(A_MINUS_B) | _ALW, _W(reg) | BW_ALU));
    writeCarrycondInstruction(ALU_OPCODE(true, A_MINUS_B, reg), true,  MICROCODE2(_E(reg) | ALB | ALS(A_MINUS_B) | _ALW | ALC, _W(reg) | BW_ALU));    
  }
  serialOutputDone();

  serialOutputBegin("SUB|SBC reg <- Rb - reg");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, B_MINUS_A, reg), MICROCODE2(_E(reg) | ALB | ALC | ALS(B_MINUS_A) | _ALW, _W(reg) | BW_ALU));
    writeCarrycondInstruction(ALU_OPCODE(true, B_MINUS_A, reg), false, MICROCODE2(_E(reg) | ALB | ALS(B_MINUS_A) | _ALW, _W(reg) | BW_ALU));
    writeCarrycondInstruction(ALU_OPCODE(true, B_MINUS_A, reg), true, MICROCODE2(_E(reg) | ALB | ALS(B_MINUS_A) | _ALW | ALC, _W(reg) | BW_ALU));    
  }
  serialOutputDone();

  serialOutputBegin("AND reg <- reg & Rb");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, A_AND_B, reg), MICROCODE2(_E(reg) | ALB | ALS(A_AND_B) | _ALW, _W(reg) | BW_ALU));
  }
  serialOutputDone();
  
  serialOutputBegin("OR reg <- reg | Rb");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, A_OR_B, reg), MICROCODE2(_E(reg) | ALB | ALS(A_OR_B) | _ALW, _W(reg) | BW_ALU));
  }
  serialOutputDone();
  
  serialOutputBegin("XOR reg <- reg ^ Rb");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, A_XOR_B, reg), MICROCODE2(_E(reg) | ALB | ALS(A_XOR_B) | _ALW, _W(reg) | BW_ALU));
  }
  serialOutputDone();

  serialOutputBegin("NOT reg <- !reg");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(false, NOT_A, reg), MICROCODE2(_E(reg) | ALS(B_MINUS_A) | _ALW, _W(reg) | BW_ALU));
  }
  serialOutputDone();

  serialOutputBegin("CMP Rb, reg");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(true, A_OR_B, reg), MICROCODE1(_E(reg) | ALC | ALB | ALS(B_MINUS_A) | _ALW));
  }
  serialOutputDone();

  serialOutputBegin("CMP reg, Rb");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(true, A_AND_B, reg), MICROCODE1(_E(reg) | ALC | ALB | ALS(A_MINUS_B) | _ALW));
  }
  serialOutputDone();

  serialOutputBegin("TST reg");
  for (uint8_t reg = Ra; reg <= Rd; reg++) {

    serialOutputStep();
    writeInstruction(ALU_OPCODE(true, NOT_A, reg), MICROCODE1(_E(reg) | ALS(A_PLUS_B) | _ALW));
  }
  serialOutputDone();

//  println("Written 60 ALU instructions.");
//  print("Writing HLT to currently unused instructions (8 total): XORC   NOTC.");
}
#endif


void setup()
{
  Serial.begin(115200);
  eeprom.setPageSize(PAGE_SIZE);
}

// the setup function runs once when you press reset or power the board
void loop() {
  println("Troy's EEPROM programmer. ");

  println("Choose ROM to program (2, 1 or 0) ... ('d' to dump)");
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
          println("");
          eeprom.dump(0, 0x7FFF);
          return;
          break;

        default:
          break;
      }
    }
  }

  print("Programming EEPROM #");
  println(ROM_NO);

  print("Pages: ");
  println(NUM_PAGES);

    int dirtyPages = 0;

    opcodeSet.clear();
#ifdef OUTPUT_ROM_FILE
    pageValid.clear(false);
    currentPage = 0;
#else
    pageValid.clear(true);
    currentPage = -1;
#endif
    
    for (; currentPage < NUM_PAGES; ++currentPage)
    {
      if (!isCheckingStage())
      {
        if (currentPage % 32 == 0)
        {
          println("");
        }
      
        if (pageValid.isSet(currentPage))
        {
          print(".");
          continue;
        }
        opcodeSet.clear();
        
        MICROCODE1(HLT);
        for (uint32_t a = (currentPage * PAGE_SIZE); a < ((currentPage + 1) * PAGE_SIZE); ++a)
        {
          int T = ((a >> 8)& 0x07);
          uint8_t eeprom_byte = (flip_active_lows(microcode[T]) >> (8*ROM_NO)) & 0xFF;
          doWrite(a, eeprom_byte);
        }

        opcodeSet.clear();
        
      }
      else
      {
        println("Checking...");      
      }

 //     println("Start Page...");      
     
      #ifdef INS_MOV
   //   println("MOV...");      
      write_MOVs();
      #endif
    
      #ifdef INS_LOD
  //    println("LOD...");      
      write_LODs();
      #endif
    
      #ifdef INS_STO
//      println("STO...");      
      write_STOs();
      #endif
      
      #ifdef INS_ALU
//      println("ALU...");      
      write_ALU_instructions();
      #endif

  //     println("End page...");      

      /// DO THE ACTUAL WRITE?
      
      if (isCheckingStage())
      {
    //    println("Checking dirty...");      
        for (int cp = 0; cp < NUM_PAGES; ++cp)
        {
          if (!pageValid.isSet(cp))
          {
            ++dirtyPages;
          }
        }

        if (dirtyPages == 0)
        {
          println("All G...");      
          break;
        }
        
        print("Writing ");
        print(dirtyPages);
        println(" changed pages...");      
        continue;
      }

#ifdef OUTPUT_ROM_FILE
      char buf[10];
      for (uint32_t cb = 0; cb < PAGE_SIZE; ++cb)
      {
        byte *b = (byte*)(pageBytes + cb);
        sprintf(buf, "%02x%02x%02x%02x", *(b + 3), *(b + 2), *(b + 1), *b);
        
        Serial.print(buf);
      }
#else
      for (uint32_t cb = 0; cb < PAGE_SIZE; ++cb)
      {        
        eeprom.write(currentPage * PAGE_SIZE + cb, pageBytes[cb]);
      }
#endif

      eeprom.endWrite();
      
      bool isValid = false;
      for (uint32_t cb = 0; cb < PAGE_SIZE; ++cb)
      {
        isValid = verify(currentPage * PAGE_SIZE + cb);
        if (!isValid)
        {
          break;
        }
      }

      if (isValid)
      {
        print("o");
      }
      else
      {
        print("X");
      }

   }
  
    eeprom.endWrite();
   
  #ifdef DUMP
  println("");
  eeprom.dump(0, TOTAL_BYTES);
  #endif

 #ifdef REPORT_UNUSED
   println("");
   println("Unused Opcodes");
   for (size_t i = 0; i < 256; ++i)
   {
      if (!opcodeSet.isSet(i))
      {
        print(i);
        print(" - ");
        println(i, BIN);
      }
   }
  #endif

 
  print("Done programming EEPROM #");
  println(ROM_NO);
}
