#pragma twice
#include "mmu.h"
#include <cstdint>
#include <iostream>

class MMU;

class CPU {
public:
  CPU(MMU &mmu);
  void reset();
  int step(); // Execute 1 instruction returns cycles

  void instr_INC(int regIndex);
  void instr_DEC(int regIndex);

  void instr_ADD(uint8_t value);
  void instr_ADC(uint8_t value);

  void instr_SUB(uint8_t value);
  void instr_SBC(uint8_t value);

  void instr_AND(uint8_t value);
  void instr_OR(uint8_t value);
  void instr_XOR(uint8_t value);
  void instr_CP(uint8_t value);

  void handleVector(uint8_t opcode);
  uint16_t getPC() const { return PC; };

  void checkInterrupts();

  // CB opcodes

  void handleCB();

  void RLC(uint8_t &reg);
  void RL(uint8_t &reg);
  void RLC_memHL();
  void RL_memHL();
  void RRC(uint8_t &reg);
  void RR(uint8_t &reg);
  void SLA(uint8_t &reg);
  void SRA(uint8_t &reg);
  void SRL(uint8_t &reg);
  void SWAP(uint8_t &reg);

  void RRC_memHL() {
    uint8_t val = read8(getHL());
    RRC(val);
    write8(getHL(), val);
  }
  void RR_memHL() {
    uint8_t val = read8(getHL());
    RR(val);
    write8(getHL(), val);
  }
  void SLA_memHL() {
    uint8_t val = read8(getHL());
    SLA(val);
    write8(getHL(), val);
  }
  void SRA_memHL() {
    uint8_t val = read8(getHL());
    SRA(val);
    write8(getHL(), val);
  }
  void SRL_memHL() {
    uint8_t val = read8(getHL());
    SRL(val);
    write8(getHL(), val);
  }
  void SWAP_memHL() {
    uint8_t val = read8(getHL());
    SWAP(val);
    write8(getHL(), val);
  }

  void BIT(uint8_t bit, uint8_t reg);
  void BIT_memHL(uint8_t bit);
  void SET(uint8_t bit, uint8_t &reg);
  void SET_memHL(uint8_t bit);
  void RES(uint8_t bit, uint8_t &reg);
  void RES_memHL(uint8_t bit);

private:
  MMU &mmu;

  int cycles;

  // 8-bit registers
  uint8_t A, B, C, D, E, H, L;
  uint8_t F; // flags: Z N H C stored in bits 7,6,5,4

  uint16_t SP; // stack pointer
  uint16_t PC; // program counter

  bool IME = false;              // Interrupt Master Enable
  bool pendingEnableIME = false; // Delayed EI enable
  bool halted = false;

  uint8_t read8(uint16_t addr);
  void write8(uint16_t addr, uint8_t val);

  uint16_t read16(uint16_t addr);
  void write16(uint16_t addr, uint16_t val);

  uint16_t pop16();
  void push16(uint16_t val);

  int execute(uint8_t opcode); // main decoder

  uint8_t &reg(int index) {
    switch (index) {
    case 0:
      return B;
    case 1:
      return C;
    case 2:
      return D;
    case 3:
      return E;
    case 4:
      return H;
    case 5:
      return L;
    case 6:
      return F;
    case 7:
      return A;
    default:
      std::cout << "Invalid reg index: " << index << "\n";
      std::exit(1);
    }
  }

  enum Flag : uint8_t {
    ZF = 0x80, // Zero
    NF = 0x40, // Subtract
    HF = 0x20, // Half Carry
    CF = 0x10  // Carry
  };

  inline void setFlag(uint8_t &F, Flag flag, bool set) {
    if (set)
      F |= flag;
    else
      F &= ~flag;
  }

  inline bool checkFlag(Flag f) { return (F & f) != 0; }

  // Helpers
  uint16_t getAF() const { return (A << 8) | F; }
  uint16_t getBC() const { return (B << 8) | C; }
  uint16_t getDE() const { return (D << 8) | E; }
  uint16_t getHL() const { return (H << 8) | L; }

  void setAF(uint16_t val) {
    A = val >> 8;
    F = val & 0xF0;
  }
  void setBC(uint16_t val) {
    B = val >> 8;
    C = val & 0xFF;
  }
  void setDE(uint16_t val) {
    D = val >> 8;
    E = val & 0xFF;
  }
  void setHL(uint16_t val) {
    H = val >> 8;
    L = val & 0xFF;
  }
};
