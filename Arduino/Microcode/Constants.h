/*
 * Troy's 8-bit computer - Arduino microcode constants
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

// control word constants (eeprom outputs)
// these define the purpose of each of the control word bits

// write to bus constants (bits 2-0 of our control word)
// this value is fed to a 74LS138 which ensures only
// one IC is outputting to the bus at any given time
static const uint32_t BW_PC  = 0b111;
static const uint32_t BW_MEM = 0b110;
static const uint32_t BW_StP = 0b101;
static const uint32_t BW_Ra  = 0b100;
static const uint32_t BW_Rb  = 0b011;
static const uint32_t BW_Rc  = 0b010;
static const uint32_t BW_Rd  = 0b001;
static const uint32_t BW_ALU = 0b000;

// ALU mode constants (bits 5-3 of our control word)
// these are passed to the ALU (74LS382) S2, S1 and S0 inputs
// inputs: 'A' from the bus. 'B' can be either Rb or 0 depending
// on the ALB flag being set
static const uint32_t ALU_OFFSET    = 3;
static const uint32_t ALU_INC_A     = 0b000 << ALU_OFFSET; // clear (set all 0's)
static const uint32_t ALU_B_MINUS_A = 0b001 << ALU_OFFSET;
static const uint32_t ALU_A_MINUS_B = 0b010 << ALU_OFFSET;
static const uint32_t ALU_A_PLUS_B  = 0b011 << ALU_OFFSET;
static const uint32_t ALU_A_XOR_B   = 0b100 << ALU_OFFSET;
static const uint32_t ALU_A_OR_B    = 0b101 << ALU_OFFSET;
static const uint32_t ALU_A_AND_B   = 0b110 << ALU_OFFSET;
static const uint32_t ALU_NOT_A     = 0b111 << ALU_OFFSET; // preset (set all 1's)

// other control word bits
// names starting with an underscore are active low
static const uint32_t  ALB  = 1 << 6;  // if set, ALU to use Rb as second input, otherwise use 0
static const uint32_t  ALC  = 1 << 7;  // set carry in for the ALU
static const uint32_t _ALW  = 1 << 8;  // (active low) accept result into ALU register and flags from ALU
static const uint32_t _RdW  = 1 << 9;  // (active low) write to register D
static const uint32_t _RcW  = 1 << 10; // (active low) write to register C
static const uint32_t _RbW  = 1 << 11; // (active low) write to register B
static const uint32_t _RaW  = 1 << 12; // (active low) write to register A
static const uint32_t _StPW = 1 << 13; // (active low) write to stack pointer
static const uint32_t _MW   = 1 << 14; // (active low) write to memory data
static const uint32_t  PGM  = 1 << 15; // program memory enable
static const uint32_t _IRW  = 1 << 16; // (active low) write to instruction register
static const uint32_t  PCC  = 1 << 17; // increment program counter
static const uint32_t _PCW  = 1 << 18; // (active low) write to program counter
static const uint32_t _MAW  = 1 << 19; // (active low) write to memory address register
static const uint32_t TBD20 = 1 << 20; // (unused) tbd
static const uint32_t TBD21 = 1 << 21; // (unused) tbd
static const uint32_t _TR   = 1 << 22; // (active low) reset microcode counter (next instruction)
static const uint32_t  HLT  = 1 << 23; // halt

static const uint32_t ACTIVE_LOW_FLAGS = (_ALW | _StPW | _PCW | _RaW | _RbW | _RcW | _RdW | _MAW | _IRW | _MW | _TR);

uint32_t inline flipActiveLows(uint32_t controlWord) {

  return controlWord ^ ACTIVE_LOW_FLAGS;
}


// EEPROM address:  14        13         12       11         10  ....   8      7   .....    0
//                  Z flag    C flag     O flag   N flag    (microtime) T              opcode

class Register
{
  public:
    static const uint8_t RA_BITS   = 0b000;
    static const uint8_t RB_BITS   = 0b001;
    static const uint8_t RC_BITS   = 0b010;
    static const uint8_t RD_BITS   = 0b011;
    static const uint8_t STP_BITS  = 0b100;
    static const uint8_t PC_BITS   = 0b101;
    static const uint8_t STPI_BITS = 0b110;
    static const uint8_t ACC_BITS  = 0b110;
    static const uint8_t IMM_BITS  = 0b111;

    static Register Ra()   { return Register(RA_BITS); }
    static Register Rb()   { return Register(RB_BITS); }
    static Register Rc()   { return Register(RC_BITS); }
    static Register Rd()   { return Register(RD_BITS); }
    static Register StP()  { return Register(STP_BITS); }
    static Register PC()   { return Register(PC_BITS); }
    static Register StPi() { return Register(STPI_BITS); }
    static Register Acc()  { return Register(ACC_BITS); }
    static Register Imm()  { return Register(IMM_BITS); }

    static const uint8_t Mask      = 0b111;
    static const uint8_t MinorMask = 0b11;

    operator uint32_t() const { return m_register; }

    uint32_t writeToBus() const
    {
      switch (m_register)
      {
        case RA_BITS:  return BW_Ra;
        case RB_BITS:  return BW_Rb;
        case RC_BITS:  return BW_Rc;
        case RD_BITS:  return BW_Rd;
        case STP_BITS: return BW_StP;
        case ACC_BITS: return BW_ALU;
        case PC_BITS:  return BW_PC;
        default:       return 0;
      }
    }

    uint32_t readFromBus() const
    {
      switch (m_register)
      {
        case RA_BITS:  return _RaW;
        case RB_BITS:  return _RbW;
        case RC_BITS:  return _RcW;
        case RD_BITS:  return _RdW;
        case STP_BITS: return _StPW;
        case PC_BITS:  return _PCW;
        default:       return 0;
      }
    }

    std::string toString() const
    {
      switch (m_register)
      {
        case RA_BITS: return "Ra";
        case RB_BITS: return "Rb";
        case RC_BITS: return "Rc";
        case RD_BITS: return "Rd";
        case STP_BITS: return "SP";
        case PC_BITS: return "PC";
        case ACC_BITS: return "Acc";
        case IMM_BITS: return "Imm";
      }
      return "Unknown";
    }
  
  public:
    Register(uint8_t reg) : m_register(reg & Mask) {}

  private:
    uint8_t m_register;
};

class OpcodeGroup
{
  public:
    static const uint32_t MOV_BITS = 0b00;
    static const uint32_t LOD_BITS = 0b01;
    static const uint32_t STO_BITS = 0b10;
    static const uint32_t ALU_BITS = 0b11;

    static const OpcodeGroup MOV() { return OpcodeGroup(MOV_BITS); }
    static const OpcodeGroup LOD() { return OpcodeGroup(LOD_BITS); }
    static const OpcodeGroup STO() { return OpcodeGroup(STO_BITS); }
    static const OpcodeGroup ALU() { return OpcodeGroup(ALU_BITS); }

    static const uint8_t Mask = 0b11;

    operator uint32_t() const { return m_group; }

    std::string toString() const
    {
      switch (m_group)
      {
        case MOV_BITS: return "mov";
        case LOD_BITS: return "lod";
        case STO_BITS: return "sto";
        case ALU_BITS: return "alu";
      }
      return "Unknown";
    }
  public:
    OpcodeGroup(uint8_t group) : m_group(group & Mask) {}

  private:
    uint8_t m_group;
};


class AluMode
{
public:
  static const uint32_t INC_A_BITS = 0b000; // clear (set all 0's)
  static const uint32_t B_MINUS_A_BITS = 0b001;
  static const uint32_t A_MINUS_B_BITS = 0b010;
  static const uint32_t A_PLUS_B_BITS = 0b011;
  static const uint32_t A_XOR_B_BITS = 0b100;
  static const uint32_t A_OR_B_BITS = 0b101;
  static const uint32_t A_AND_B_BITS = 0b110;
  static const uint32_t NOT_A_BITS = 0b111; // preset (set all 1's)

  static AluMode INC_A() { return AluMode(INC_A_BITS); }
  static AluMode B_MINUS_A() { return AluMode(B_MINUS_A_BITS); }
  static AluMode A_MINUS_B() { return AluMode(A_MINUS_B_BITS); }
  static AluMode A_PLUS_B() { return AluMode(A_PLUS_B_BITS); }
  static AluMode A_XOR_B() { return AluMode(A_XOR_B_BITS); }
  static AluMode A_OR_B() { return AluMode(A_OR_B_BITS); }
  static AluMode A_AND_B() { return AluMode(A_AND_B_BITS); }
  static AluMode NOT_A() { return AluMode(NOT_A_BITS); }

  static const uint8_t Mask = 0b111;

  operator uint32_t() const { return m_mode; }

  std::string toString() const
  {
    switch (m_mode)
    {
      case INC_A_BITS: return "clear";
      case B_MINUS_A_BITS: return "Rb sub";
      case A_MINUS_B_BITS: return "sub Rb from";
      case A_PLUS_B_BITS: return "add Rb";
      case A_XOR_B_BITS: return "xor Rb";
      case A_OR_B_BITS: return "or Rb";
      case A_AND_B_BITS: return "and Rb";
      case NOT_A_BITS: return "set";
    }
    return "Unknown";
  }


public:
  AluMode(uint8_t mode) : m_mode(mode & Mask) {}

private:
  uint8_t m_mode;
};


class Opcode
{
  protected:
    // general opcode layout (8 bits)
    // bit:   |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
    // value: |-- opcode group  --|--- destination register ----|-----  source register  -----|

    static const uint8_t GroupOffset  = 6;
    static const uint8_t DstRegOffset = 3;
    static const uint8_t SrcRegOffset = 0;

    static const uint8_t GroupMask  = OpcodeGroup::Mask << GroupOffset;
    static const uint8_t DstRegMask = Register::Mask << DstRegOffset;
    static const uint8_t SrcRegMask = Register::Mask << SrcRegOffset;

  public:
    static const uint32_t Mask = 0xff;

    Opcode(uint8_t opcode) : m_opcode(opcode) {}
    Opcode(const OpcodeGroup &group,
           const Register &destReg,
           const Register &srcReg)
      : m_opcode((group << GroupOffset) |
                 (destReg << DstRegOffset) | 
                 (srcReg << SrcRegOffset))
    {
    }
    virtual ~Opcode() {}
    
    OpcodeGroup group() const { return OpcodeGroup(m_opcode >> GroupOffset); }
    Register destReg() const { return Register(m_opcode >> DstRegOffset); }
    Register srcReg() const { return Register(m_opcode >> SrcRegOffset); }

    std::string bitsToString() const
    {
      std::string formatted;
      for (size_t i = 0; i < 8; ++i)
      {
        if (insertSpace(i))
        {
          formatted += ' ';
        }
        formatted += m_opcode & (0x80 >> i) ? '1' : '0';
      }
      return formatted;
    }

    virtual std::string describe() const
    {
      std::string desc;
      desc += group().toString() + " ";
      desc += destReg().toString() + " ";
      desc += srcReg().toString();
      return desc;
    }

    operator uint32_t() const { return m_opcode; }

  protected:
    virtual bool insertSpace(size_t pos) const
    {
      switch (pos)
      {
        case 2:
        case 5:
          return true;

        default:
          return false;
      }
    }

  protected:
    uint8_t m_opcode;
};

class AluOpcode : public Opcode
{
  // ALU opcode layout (8 bits)
  // bit:   |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
  // value: |    1   grp   1    |use carry|---------  alu mode  --------|---- register -----|

  static const uint8_t CarryOffset   = 5;
  static const uint8_t AluModeOffset = 2;
  static const uint8_t RegOffset     = 0;

  static const uint8_t CarryMask     = 1 << CarryOffset;
  static const uint8_t AluModeMask   = AluMode::Mask << AluModeOffset;
  static const uint8_t RegMask       = Register::MinorMask << RegOffset;

  public:
    enum CarryFlag
    {
      IgnoreCarry,
      UseCarry,
    };

    AluOpcode(uint8_t opcode) : Opcode(opcode) {}
    AluOpcode(CarryFlag carryFlag,
              const AluMode& aluMode,
              const Register& reg)
      : Opcode((OpcodeGroup::ALU() << GroupOffset) |
               ((carryFlag & 0x1) << CarryOffset) |
               (aluMode << AluModeOffset) |
               ((reg & RegMask) << RegOffset))
    {

    }

    AluMode aluMode() const { return AluMode(m_opcode >> AluModeOffset); }
    Register aluReg() const { return Register((m_opcode & RegMask) >> RegOffset); }
    bool useCarry() const { return m_opcode & CarryMask; }

    std::string describe() const override
    {
      std::string desc;
      desc += aluMode().toString() + " ";
      desc += aluReg().toString();
      desc += useCarry() ? " with carry " : "";
      desc += " => " + aluReg().toString();
      return desc;
    }

  protected:
    bool insertSpace(size_t pos) const override
    {
      switch (pos)
      {
        case 2:
        case 3:
        case 6:
          return true;

        default:
          return false;
      }
    }
};


class EepromAddress
{
  // EEPROM address layout (15 bits)
  // bit:   |   14    |   13    |   12    |   11    |   10    |    9    |    8    |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
  // value: |zero flag|cary flag|oflw flag|neg flag |-----  microcode step -------|----------------------------------- op code  ----------------------------------|

  private:
    static const uint8_t OpcodeOffset    = 0;
    static const uint8_t MicrotimeOffset = 8;
    static const uint8_t FlagsOffset     = 11;

    static const uint16_t OpcodeMask    = Opcode::Mask << OpcodeOffset;
    static const uint16_t MicrotimeMask = 0x7  << MicrotimeOffset;
    static const uint16_t FlagsMask     = 0xf  << FlagsOffset;

  public:
    static const uint8_t NegativeFlag = 1 << 0;
    static const uint8_t OverflowFlag = 1 << 1;
    static const uint8_t CarryFlag    = 1 << 2;
    static const uint8_t ZeroFlag     = 1 << 3;

    static const uint16_t TOTAL_BYTES = 1 << 15;

    EepromAddress(uint16_t address) : m_address(address) {}
    EepromAddress(uint8_t flags, uint8_t microtime, const Opcode &opcode)
    : m_address(((flags << FlagsOffset) & FlagsMask) |
                ((microtime << MicrotimeOffset) & MicrotimeMask) |
                ((opcode << OpcodeOffset) & OpcodeMask))
    {
    }

    uint8_t flags() const { return (m_address & FlagsMask) >> FlagsOffset; }
    uint8_t microtime() const { return (m_address & MicrotimeMask) >> MicrotimeOffset; }
    const Opcode &opcode() const
    {
      static Opcode oc(0);

      oc = (m_address & OpcodeMask) >> OpcodeOffset;

      if (oc.group() == OpcodeGroup::ALU())
      {
        static AluOpcode aluoc(0);
        aluoc = (uint32_t)oc;
        return aluoc;
      }

      return oc;
    }

    bool isNegativeFlagSet() const { return m_address & (NegativeFlag << FlagsOffset); }
    bool isOverflowFlagSet() const { return m_address & (OverflowFlag << FlagsOffset); }
    bool isCarryFlagSet() const { return m_address & (CarryFlag << FlagsOffset); }
    bool isZeroFlagSet() const { return m_address & (ZeroFlag << FlagsOffset); }

    std::string toString() const
    {
      std::string str;
      str += isZeroFlagSet() ? "Z " : "- ";
      str += isCarryFlagSet() ? "C " : "- ";
      str += isOverflowFlagSet() ? "O " : "- ";
      str += isNegativeFlagSet() ? "N " : "- ";
      str += "(" + opcode().describe() + ") ";
      str += microtime();
      return str;
    }

    operator uint16_t() const { return m_address; }

  private:
    uint16_t m_address;
};
