#include "cpu.h"
#include <cstdio>
#include <iostream>

void CPU::instr_INC(int regIndex) {
  uint8_t &r = reg(regIndex);
  setFlag(F, HF, (r & 0x0F) == 0x0F); // half-carry
  r += 1;
  setFlag(F, ZF, r == 0);
  setFlag(F, NF, false);

  std::cout << "[CPU] - Incrementing... " << "\n";

  PC += 1;
  cycles = 4;
}

void CPU::instr_DEC(int regIndex) {
  uint8_t &r = reg(regIndex);
  setFlag(F, HF, (r & 0x0F) == 0x00); // half-borrow
  r -= 1;
  setFlag(F, ZF, r == 0);
  setFlag(F, NF, true);

  std::cout << "[CPU] - Derementing..." << "\n";

  PC += 1;
  cycles = 4;
}

void CPU::instr_ADD(uint8_t value) {
  uint16_t result = A + value;

  setFlag(F, ZF, (result & 0xFF) == 0);
  setFlag(F, NF, false);
  setFlag(F, HF, ((A & 0x0F) + (value & 0x0F)) > 0x0F);
  setFlag(F, CF, result > 0xFF);

  A = result & 0xFF;

  std::cout << "[CPU] - Adding...\n";

  cycles = 4;
}

void CPU::instr_ADC(uint8_t value) {
  uint16_t result = A + value + (F & CF ? 1 : 0);
  setFlag(F, ZF, (result & 0xFF) == 0);
  setFlag(F, NF, false);
  setFlag(F, HF, ((A & 0xF) + (value & 0xF) + (F & CF ? 1 : 0)) > 0xF);
  setFlag(F, CF, result > 0xFF);
  A = result & 0xFF;

  cycles = 4;
}

void CPU::instr_SUB(uint8_t value) {
  uint16_t result = A - value;
  setFlag(F, ZF, (result & 0xFF) == 0);
  setFlag(F, NF, true);
  setFlag(F, HF, (A & 0xF) < (value & 0xF));
  setFlag(F, CF, A < value);
  A = result & 0xFF;

  cycles = 4;
}

void CPU::instr_SBC(uint8_t value) {
  uint16_t result = A - value - (F & CF ? 1 : 0);
  setFlag(F, ZF, (result & 0xFF) == 0);
  setFlag(F, NF, true);
  setFlag(F, HF, (A & 0xF) < ((value & 0xF) + (F & CF ? 1 : 0)));
  setFlag(F, CF, A < value + (F & CF ? 1 : 0));
  A = result & 0xFF;

  cycles = 4;
}

void CPU::instr_AND(uint8_t value) {
  A &= value;
  setFlag(F, ZF, A == 0);
  setFlag(F, NF, false);
  setFlag(F, HF, true);
  setFlag(F, CF, false);

  cycles = 4;
}

void CPU::instr_OR(uint8_t value) {
  A |= value;
  setFlag(F, ZF, A == 0);
  setFlag(F, NF, false);
  setFlag(F, HF, false);
  setFlag(F, CF, false);

  cycles = 4;
}

void CPU::instr_XOR(uint8_t value) {
  A ^= value;

  // Flags:
  setFlag(F, ZF, A == 0); // Zero flag
  setFlag(F, NF, false);  // Subtract flag
  setFlag(F, HF, false);  // Half-carry flag
  setFlag(F, CF, false);  // Carry flag

  cycles = 4;
}

void CPU::instr_CP(uint8_t value) {
  uint16_t result = A - value;
  setFlag(F, ZF, (result & 0xFF) == 0);
  setFlag(F, NF, true);
  setFlag(F, HF, (A & 0xF) < (value & 0xF));
  setFlag(F, CF, A < value);

  cycles = 4;
}

// CB Opcodes

void CPU::RLC(uint8_t &reg) {
  uint8_t carry = (reg & 0x80) >> 7;
  reg = (reg << 1) | carry;
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-RLC --> " << (int)reg << "\n";
}

// RL: Rotate Left through Carry
void CPU::RL(uint8_t &reg) {
  uint8_t carry_in = (F & CF) ? 1 : 0;
  uint8_t carry = (reg & 0x80) >> 7;
  reg = (reg << 1) | carry_in;
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-RL --> " << (int)reg << "\n";
}

// RRC: Rotate Right Circular
void CPU::RRC(uint8_t &reg) {
  uint8_t carry = reg & 0x01;
  reg = (reg >> 1) | (carry << 7);
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-RRC --> " << (int)reg << "\n";
}

// RR: Rotate Right through Carry
void CPU::RR(uint8_t &reg) {
  uint8_t carry_in = (F & CF) ? 0x80 : 0;
  uint8_t carry = reg & 0x01;
  reg = (reg >> 1) | carry_in;
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-RR --> " << (int)reg << "\n";
}

// SLA: Shift Left Arithmetic
void CPU::SLA(uint8_t &reg) {
  uint8_t carry = (reg & 0x80) >> 7;
  reg <<= 1;
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-SLA --> " << (int)reg << "\n";
}

// SRA: Shift Right Arithmetic (preserve MSB)
void CPU::SRA(uint8_t &reg) {
  uint8_t carry = reg & 0x01;
  reg = (reg >> 1) | (reg & 0x80);
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-SRA --> " << (int)reg << "\n";
}

// SRL: Shift Right Logical (fill MSB with 0)
void CPU::SRL(uint8_t &reg) {
  uint8_t carry = reg & 0x01;
  reg >>= 1;
  F = 0;
  if (reg == 0)
    F |= ZF;
  if (carry)
    F |= CF;
  std::cout << "[CPU] CB-SRL --> " << (int)reg << "\n";
}

// SWAP: Swap high/low nibbles
void CPU::SWAP(uint8_t &reg) {
  reg = (reg << 4) | (reg >> 4);
  F = 0;
  if (reg == 0)
    F |= ZF;
  std::cout << "[CPU] CB-SWAP --> " << (int)reg << "\n";
}

// BIT: Test bit (no modification)
void CPU::BIT(uint8_t bit, uint8_t reg) {
  F &= CF; // Preserve Carry flag
  F |= HF;
  if (!(reg & (1 << bit)))
    F |= ZF;
  std::cout << "[CPU] CB-BIT " << (int)reg << " bit " << (int)bit << "\n";
}

// SET: Set bit
void CPU::SET(uint8_t bit, uint8_t &reg) {
  reg |= (1 << bit);
  std::cout << "[CPU] CB-SET " << (int)reg << " bit " << (int)bit << "\n";
}

// RES: Reset bit
void CPU::RES(uint8_t bit, uint8_t &reg) {
  reg &= ~(1 << bit);
  std::cout << "[CPU] CB-RES " << (int)reg << " bit " << (int)bit << "\n";
}

void CPU::RLC_memHL() {
  uint8_t val = read8(getHL());
  RLC(val);
  write8(getHL(), val);
  std::cout << "[CPU] CB-RLC (HL)\n";
}

void CPU::RL_memHL() {
  uint8_t val = read8(getHL());
  RL(val);
  write8(getHL(), val);
  std::cout << "[CPU] CB-RL (HL)\n";
}

void CPU::BIT_memHL(uint8_t bit) {
  uint8_t val = read8(getHL());
  BIT(bit, val);
  std::cout << "[CPU] CB-BIT (HL) bit " << (int)bit << "\n";
}

void CPU::SET_memHL(uint8_t bit) {
  uint8_t val = read8(getHL());
  SET(bit, val);
  write8(getHL(), val);
  std::cout << "[CPU] CB-SET (HL) bit " << (int)bit << "\n";
}

void CPU::RES_memHL(uint8_t bit) {
  uint8_t val = read8(getHL());
  RES(bit, val);
  write8(getHL(), val);
  std::cout << "[CPU] CB-RES (HL) bit " << (int)bit << "\n";
}
// #########################################################################################################################################################################
//  handlers
// #########################################################################################################################################################################
void CPU::handleVector(uint8_t opcode) {
  uint16_t target = opcode & 0x38;
  std::printf("[CPU] %04X: RST 0x%02X\n", PC, target);
  push16(PC + 1);
  PC = target;
  cycles = 16;
  return;
}

bool isMemoryHL(uint8_t cb_opcode) {
  uint8_t reg_code = cb_opcode & 0x07; // Extract rrr (lowest 3 bits)
  return reg_code == 0x06;             // 6 = (HL)
}

void CPU::handleCB() {
  uint8_t cb_opcode = read8(PC + 1);
  PC += 2;
  if (isMemoryHL(cb_opcode)) {
    cycles = 16;
  } else {
    cycles = 8;
  }

  switch (cb_opcode) {
    // RLC group (Rotate Left Circular)
  case 0x00:
    RLC(B);
    break;
  case 0x01:
    RLC(C);
    break;
  case 0x02:
    RLC(D);
    break;
  case 0x03:
    RLC(E);
    break;
  case 0x04:
    RLC(H);
    break;
  case 0x05:
    RLC(L);
    break;
  case 0x06:
    RLC_memHL();
    break;
  case 0x07:
    RLC(A);
    break;

  case 0x08:
    RRC(B);
    break;
  case 0x09:
    RRC(C);
    break;
  case 0x0A:
    RRC(D);
    break;
  case 0x0B:
    RRC(E);
    break;
  case 0x0C:
    RRC(H);
    break;
  case 0x0D:
    RRC(L);
    break;
  case 0x0E:
    RRC_memHL();
    break;
  case 0x0F:
    RRC(A);
    break;

  case 0x10:
    RL(B);
    break;
  case 0x11:
    RL(C);
    break;
  case 0x12:
    RL(D);
    break;
  case 0x13:
    RL(E);
    break;
  case 0x14:
    RL(H);
    break;
  case 0x15:
    RL(L);
    break;
  case 0x16:
    RL_memHL();
    break;
  case 0x17:
    RL(A);
    break;

  case 0x18:
    RR(B);
    break;
  case 0x19:
    RR(C);
    break;
  case 0x1A:
    RR(D);
    break;
  case 0x1B:
    RR(E);
    break;
  case 0x1C:
    RR(H);
    break;
  case 0x1D:
    RR(L);
    break;
  case 0x1E:
    RR_memHL();
    break;
  case 0x1F:
    RR(A);
    break;

  case 0x20:
    SLA(B);
    break;
  case 0x21:
    SLA(C);
    break;
  case 0x22:
    SLA(D);
    break;
  case 0x23:
    SLA(E);
    break;
  case 0x24:
    SLA(H);
    break;
  case 0x25:
    SLA(L);
    break;
  case 0x26:
    SLA_memHL();
    break;
  case 0x27:
    SLA(A);
    break;

  case 0x28:
    SRA(B);
    break;
  case 0x29:
    SRA(C);
    break;
  case 0x2A:
    SRA(D);
    break;
  case 0x2B:
    SRA(E);
    break;
  case 0x2C:
    SRA(H);
    break;
  case 0x2D:
    SRA(L);
    break;
  case 0x2E:
    SRA_memHL();
    break;
  case 0x2F:
    SRA(A);
    break;

  case 0x30:
    SWAP(B);
    break;
  case 0x31:
    SWAP(C);
    break;
  case 0x32:
    SWAP(D);
    break;
  case 0x33:
    SWAP(E);
    break;
  case 0x34:
    SWAP(H);
    break;
  case 0x35:
    SWAP(L);
    break;
  case 0x36:
    SWAP_memHL();
    break;
  case 0x37:
    SWAP(A);
    break;

  case 0x38:
    SRL(B);
    break;
  case 0x39:
    SRL(C);
    break;
  case 0x3A:
    SRL(D);
    break;
  case 0x3B:
    SRL(E);
    break;
  case 0x3C:
    SRL(H);
    break;
  case 0x3D:
    SRL(L);
    break;
  case 0x3E:
    SRL_memHL();
    break;
  case 0x3F:
    SRL(A);
    break;

  case 0x40:
    BIT(0, B);
    break;
  case 0x41:
    BIT(0, C);
    break;
  case 0x42:
    BIT(0, D);
    break;
  case 0x43:
    BIT(0, E);
    break;
  case 0x44:
    BIT(0, H);
    break;
  case 0x45:
    BIT(0, L);
    break;
  case 0x46:
    BIT_memHL(0);
    break;
  case 0x47:
    BIT(0, A);
    break;

  case 0x48:
    BIT(1, B);
    break;
  case 0x49:
    BIT(1, C);
    break;
  case 0x4A:
    BIT(1, D);
    break;
  case 0x4B:
    BIT(1, E);
    break;
  case 0x4C:
    BIT(1, H);
    break;
  case 0x4D:
    BIT(1, L);
    break;
  case 0x4E:
    BIT_memHL(1);
    break;
  case 0x4F:
    BIT(1, A);
    break;

  case 0x50:
    BIT(2, B);
    break;
  case 0x51:
    BIT(2, C);
    break;
  case 0x52:
    BIT(2, D);
    break;
  case 0x53:
    BIT(2, E);
    break;
  case 0x54:
    BIT(2, H);
    break;
  case 0x55:
    BIT(2, L);
    break;
  case 0x56:
    BIT_memHL(2);
    break;
  case 0x57:
    BIT(2, A);
    break;

  case 0x58:
    BIT(3, B);
    break;
  case 0x59:
    BIT(3, C);
    break;
  case 0x5A:
    BIT(3, D);
    break;
  case 0x5B:
    BIT(3, E);
    break;
  case 0x5C:
    BIT(3, H);
    break;
  case 0x5D:
    BIT(3, L);
    break;
  case 0x5E:
    BIT_memHL(3);
    break;
  case 0x5F:
    BIT(3, A);
    break;

  case 0x60:
    BIT(4, B);
    break;
  case 0x61:
    BIT(4, C);
    break;
  case 0x62:
    BIT(4, D);
    break;
  case 0x63:
    BIT(4, E);
    break;
  case 0x64:
    BIT(4, H);
    break;
  case 0x65:
    BIT(4, L);
    break;
  case 0x66:
    BIT_memHL(4);
    break;
  case 0x67:
    BIT(4, A);
    break;

  case 0x68:
    BIT(5, B);
    break;
  case 0x69:
    BIT(5, C);
    break;
  case 0x6A:
    BIT(5, D);
    break;
  case 0x6B:
    BIT(5, E);
    break;
  case 0x6C:
    BIT(5, H);
    break;
  case 0x6D:
    BIT(5, L);
    break;
  case 0x6E:
    BIT_memHL(5);
    break;
  case 0x6F:
    BIT(5, A);
    break;

  case 0x70:
    BIT(6, B);
    break;
  case 0x71:
    BIT(6, C);
    break;
  case 0x72:
    BIT(6, D);
    break;
  case 0x73:
    BIT(6, E);
    break;
  case 0x74:
    BIT(6, H);
    break;
  case 0x75:
    BIT(6, L);
    break;
  case 0x76:
    BIT_memHL(6);
    break;
  case 0x77:
    BIT(6, A);
    break;

  case 0x78:
    BIT(7, B);
    break;
  case 0x79:
    BIT(7, C);
    break;
  case 0x7A:
    BIT(7, D);
    break;
  case 0x7B:
    BIT(7, E);
    break;
  case 0x7C:
    BIT(7, H);
    break;
  case 0x7D:
    BIT(7, L);
    break;
  case 0x7E:
    BIT_memHL(7);
    break;
  case 0x7F:
    BIT(7, A);
    break;

  case 0x80:
    RES(0, B);
    break;
  case 0x81:
    RES(0, C);
    break;
  case 0x82:
    RES(0, D);
    break;
  case 0x83:
    RES(0, E);
    break;
  case 0x84:
    RES(0, H);
    break;
  case 0x85:
    RES(0, L);
    break;
  case 0x86:
    RES_memHL(0);
    break;
  case 0x87:
    RES(0, A);
    break;

  case 0x88:
    RES(1, B);
    break;
  case 0x89:
    RES(1, C);
    break;
  case 0x8A:
    RES(1, D);
    break;
  case 0x8B:
    RES(1, E);
    break;
  case 0x8C:
    RES(1, H);
    break;
  case 0x8D:
    RES(1, L);
    break;
  case 0x8E:
    RES_memHL(1);
    break;
  case 0x8F:
    RES(1, A);
    break;

  case 0x90:
    RES(2, B);
    break;
  case 0x91:
    RES(2, C);
    break;
  case 0x92:
    RES(2, D);
    break;
  case 0x93:
    RES(2, E);
    break;
  case 0x94:
    RES(2, H);
    break;
  case 0x95:
    RES(2, L);
    break;
  case 0x96:
    RES_memHL(2);
    break;
  case 0x97:
    RES(2, A);
    break;

  case 0x98:
    RES(3, B);
    break;
  case 0x99:
    RES(3, C);
    break;
  case 0x9A:
    RES(3, D);
    break;
  case 0x9B:
    RES(3, E);
    break;
  case 0x9C:
    RES(3, H);
    break;
  case 0x9D:
    RES(3, L);
    break;
  case 0x9E:
    RES_memHL(3);
    break;
  case 0x9F:
    RES(3, A);
    break;

  case 0xA0:
    RES(4, B);
    break;
  case 0xA1:
    RES(4, C);
    break;
  case 0xA2:
    RES(4, D);
    break;
  case 0xA3:
    RES(4, E);
    break;
  case 0xA4:
    RES(4, H);
    break;
  case 0xA5:
    RES(4, L);
    break;
  case 0xA6:
    RES_memHL(4);
    break;
  case 0xA7:
    RES(4, A);
    break;

  case 0xA8:
    RES(5, B);
    break;
  case 0xA9:
    RES(5, C);
    break;
  case 0xAA:
    RES(5, D);
    break;
  case 0xAB:
    RES(5, E);
    break;
  case 0xAC:
    RES(5, H);
    break;
  case 0xAD:
    RES(5, L);
    break;
  case 0xAE:
    RES_memHL(5);
    break;
  case 0xAF:
    RES(5, A);
    break;

  case 0xB0:
    RES(6, B);
    break;
  case 0xB1:
    RES(6, C);
    break;
  case 0xB2:
    RES(6, D);
    break;
  case 0xB3:
    RES(6, E);
    break;
  case 0xB4:
    RES(6, H);
    break;
  case 0xB5:
    RES(6, L);
    break;
  case 0xB6:
    RES_memHL(6);
    break;
  case 0xB7:
    RES(6, A);
    break;

  case 0xB8:
    RES(7, B);
    break;
  case 0xB9:
    RES(7, C);
    break;
  case 0xBA:
    RES(7, D);
    break;
  case 0xBB:
    RES(7, E);
    break;
  case 0xBC:
    RES(7, H);
    break;
  case 0xBD:
    RES(7, L);
    break;
  case 0xBE:
    RES_memHL(7);
    break;
  case 0xBF:
    RES(7, A);
    break;

  case 0xC0:
    SET(0, B);
    break;
  case 0xC1:
    SET(0, C);
    break;
  case 0xC2:
    SET(0, D);
    break;
  case 0xC3:
    SET(0, E);
    break;
  case 0xC4:
    SET(0, H);
    break;
  case 0xC5:
    SET(0, L);
    break;
  case 0xC6:
    SET_memHL(0);
    break;
  case 0xC7:
    SET(0, A);
    break;

  case 0xC8:
    SET(1, B);
    break;
  case 0xC9:
    SET(1, C);
    break;
  case 0xCA:
    SET(1, D);
    break;
  case 0xCB:
    SET(1, E);
    break;
  case 0xCC:
    SET(1, H);
    break;
  case 0xCD:
    SET(1, L);
    break;
  case 0xCE:
    SET_memHL(1);
    break;
  case 0xCF:
    SET(1, A);
    break;

  case 0xD0:
    SET(2, B);
    break;
  case 0xD1:
    SET(2, C);
    break;
  case 0xD2:
    SET(2, D);
    break;
  case 0xD3:
    SET(2, E);
    break;
  case 0xD4:
    SET(2, H);
    break;
  case 0xD5:
    SET(2, L);
    break;
  case 0xD6:
    SET_memHL(2);
    break;
  case 0xD7:
    SET(2, A);
    break;

  case 0xD8:
    SET(3, B);
    break;
  case 0xD9:
    SET(3, C);
    break;
  case 0xDA:
    SET(3, D);
    break;
  case 0xDB:
    SET(3, E);
    break;
  case 0xDC:
    SET(3, H);
    break;
  case 0xDD:
    SET(3, L);
    break;
  case 0xDE:
    SET_memHL(3);
    break;
  case 0xDF:
    SET(3, A);
    break;

  case 0xE0:
    SET(4, B);
    break;
  case 0xE1:
    SET(4, C);
    break;
  case 0xE2:
    SET(4, D);
    break;
  case 0xE3:
    SET(4, E);
    break;
  case 0xE4:
    SET(4, H);
    break;
  case 0xE5:
    SET(4, L);
    break;
  case 0xE6:
    SET_memHL(4);
    break;
  case 0xE7:
    SET(4, A);
    break;

  case 0xE8:
    SET(5, B);
    break;
  case 0xE9:
    SET(5, C);
    break;
  case 0xEA:
    SET(5, D);
    break;
  case 0xEB:
    SET(5, E);
    break;
  case 0xEC:
    SET(5, H);
    break;
  case 0xED:
    SET(5, L);
    break;
  case 0xEE:
    SET_memHL(5);
    break;
  case 0xEF:
    SET(5, A);
    break;

  case 0xF0:
    SET(6, B);
    break;
  case 0xF1:
    SET(6, C);
    break;
  case 0xF2:
    SET(6, D);
    break;
  case 0xF3:
    SET(6, E);
    break;
  case 0xF4:
    SET(6, H);
    break;
  case 0xF5:
    SET(6, L);
    break;
  case 0xF6:
    SET_memHL(6);
    break;
  case 0xF7:
    SET(6, A);
    break;

  case 0xF8:
    SET(7, B);
    break;
  case 0xF9:
    SET(7, C);
    break;
  case 0xFA:
    SET(7, D);
    break;
  case 0xFB:
    SET(7, E);
    break;
  case 0xFC:
    SET(7, H);
    break;
  case 0xFD:
    SET(7, L);
    break;
  case 0xFE:
    SET_memHL(7);
    break;
  case 0xFF:
    SET(7, A);
    break;

  default:
    std::printf("[CPU] Unknown CB opcode: 0x%02X\n", cb_opcode);
    break;
  }
  return;
}

int CPU::execute(uint8_t opcode) {

  if (opcode >= 0x40 && opcode <= 0x7F) {
    int dest = (opcode >> 3) & 0x07;

    int src = opcode & 0x07;
    uint16_t hlAddr = getHL();
    uint8_t value;

    if (opcode == 0x76) { // HALT
      uint8_t IE = mmu.read8(0xFFFF);
      uint8_t IF = mmu.read8(0xFF0F);
      if (IME || !(IE & IF)) {
        // Normal HALT
        halted = true;
        PC += 1;
        cycles = 4;
      } else { // bug or what?
        std::printf("[CPU] %04X: HALT bug detected (IME=0, pending IF)\n", PC);
      }
    }

    if (src == 6) { // Source is (HL)
      value = read8(hlAddr);
    } else {
      value = reg(src);
    }

    if (dest == 6) { // Destination is (HL)
      write8(hlAddr, value);
    } else {
      reg(dest) = value;
    }

    std::printf("[CPU] %04X: LD %c, %c\n", PC, "BCDEHLA?"[dest],
                "BCDEHLA?"[src]);

    PC += 1;
    cycles = (src == 6 || dest == 6) ? 8 : 4; // for hl
  } // handler
  else {

    switch (opcode) {

    case 0x00: { // NOP
      std::printf("[CPU] %04X: NOP\n", PC);
      PC += 1;
      cycles = 4;
      break;
    }
    case 0x01: // LD BC, d16
    {
      uint16_t val = read16(PC + 1);
      std::printf("[CPU] %04X: LD BC, 0x%04X\n", PC, val);
      setBC(val);
      PC += 3;
      cycles = 12;
      break;
    }
    case 0x02: // LD (BC), A
    {
      uint16_t addr = getBC();
      write8(addr, A);
      std::printf("[CPU] %04X: LD (BC), A ; [0x%04X] = 0x%02X\n", PC, addr, A);
      setBC(addr);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x03: {
      std::printf("[CPU] %04X: INC BC\n", PC);
      setBC(getBC() + 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x04:
      instr_INC(0);
      break;
    case 0x05:
      instr_DEC(0);
      break;
    case 0x06: // LD B, imm8
    {
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD B, 0x%02X\n", PC, val);
      B = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x07: { // RLCA
      std::printf("[CPU] %04X: RLCA\n", PC);
      uint8_t carry = (A & 0x80) >> 7;
      A = (A << 1) | carry;
      F = carry ? 0x10 : 0x00;
      PC += 1;
      cycles = 4;
      break;
    }

    case 0x08: // LD (a16), SP
    {
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: LD (0x%04X), SP ; SP=0x%04X\n", PC, addr, SP);
      write16(addr, SP);
      PC += 3;
      cycles = 20;
      break;
    }
    case 0x09: // ADD HL, BC
    {
      uint32_t result = getHL() + getBC();
      std::printf("[CPU] %04X: ADD HL, BC ; HL += 0x%04X\n", PC, getBC());

      F &= 0x80; // Preserve Z, clear N
      if ((getHL() & 0x0FFF) + (getBC() & 0x0FFF) > 0x0FFF)
        F |= 0x20; // H
      if (result > 0xFFFF)
        F |= 0x10; // C

      setHL(result & 0xFFFF);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x0A: // LD A, (BC)
    {
      uint16_t addr = getBC();
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (BC) ; A = [0x%04X] = 0x%02X\n", PC, addr,
                  A);
      setBC(addr);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x0B: {
      std::printf("[CPU] %04X: DEC BC\n", PC);
      setBC(getBC() - 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x0C:
      instr_INC(1);
      break;
    case 0x0D:
      instr_DEC(1);
      break;
    case 0x0E: // LD C, imm8
    {
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD C, 0x%02X\n", PC, val);
      C = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x0F: { // RRCA
      std::printf("[CPU] %04X: RRCA\n", PC);
      uint8_t carry = A & 0x01;
      A = (A >> 1) | (carry << 7);
      F = carry ? 0x10 : 0x00;
      PC += 1;
      cycles = 4;
      break;
    }

    case 0x10: { // STOP (0x10 0x00)
      uint8_t stop_byte = read8(PC + 1);
      if (stop_byte != 0x00) {
        std::cerr
            << "[CPU] %04X: STOP instruction with unexpected byte: 0x%02X\n",
            PC, stop_byte;
      }
      std::printf(
          "[CPU] %04X: STOP (Emulator stops, waiting for button input)\n", PC);
      halted = true;
      PC += 2;
      cycles = 4;
      break;
    }
    case 0x11: // LD DE, d16
    {
      uint16_t val = read16(PC + 1);
      std::printf("[CPU] %04X: LD DE, 0x%04X\n", PC, val);
      setDE(val);
      PC += 3;
      cycles = 12;
      break;
    }
    case 0x12: // LD (DE), A
    {
      uint16_t addr = getDE();
      write8(addr, A);
      std::printf("[CPU] %04X: LD (DE), A ; [0x%04X] = 0x%02X\n", PC, addr, A);
      setDE(addr);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x13: {
      std::printf("[CPU] %04X: INC DE\n", PC);
      setDE(getDE() + 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x14:
      instr_INC(2);
      break;
    case 0x15:
      instr_DEC(2);
      break;
    case 0x16: { // LD D, imm8
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD D, 0x%02X\n", PC, val);
      D = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x17: { // RLA
      std::printf("[CPU] %04X: RLA\n", PC);
      uint8_t carry_in = (F & 0x10) ? 1 : 0;
      uint8_t carry_out = (A & 0x80) >> 7;
      A = (A << 1) | carry_in;
      F = carry_out ? 0x10 : 0x00;
      PC += 1;
      cycles = 4;
      break;
    }

    case 0x18: { // JR r8
      int8_t offset = static_cast<int8_t>(read8(PC + 1));
      std::printf("[CPU] %04X: JR %d\n", PC, offset);
      if (!checkFlag(ZF)) {
        PC += offset + 2;

      } else {
        PC += 2;
      }
      cycles = 12;
      break;
    }
    case 0x19: // ADD HL, DE
    {
      uint32_t result = getHL() + getDE();
      std::printf("[CPU] %04X: ADD HL, DE ; HL += 0x%04X\n", PC, getDE());

      F &= 0x80;
      if ((getHL() & 0x0FFF) + (getDE() & 0x0FFF) > 0x0FFF)
        F |= 0x20;
      if (result > 0xFFFF)
        F |= 0x10;

      setHL(result & 0xFFFF);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x1A: // LD A, (DE)
    {
      uint16_t addr = getDE();
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (DE) ; A = [0x%04X] = 0x%02X\n", PC, addr,
                  A);
      setDE(addr);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x1B: {
      std::printf("[CPU] %04X: DEC DE\n", PC);
      setDE(getDE() - 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x1C:
      instr_INC(3);
      break;
    case 0x1D:
      instr_DEC(3);
      break;
    case 0x1E: { // LD E, imm8
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD E, 0x%02X\n", PC, val);
      E = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x1F: { // RRA
      std::printf("[CPU] %04X: RRA\n", PC);
      uint8_t carry_in = (F & 0x10) ? 0x80 : 0x00;
      uint8_t carry_out = A & 0x01;
      A = (A >> 1) | carry_in;
      F = carry_out ? 0x10 : 0x00;
      PC += 1;
      cycles = 4;
      break;
    }

    case 0x20: { // JR NZ,r8
      int8_t offset = static_cast<int8_t>(read8(PC + 1));
      std::printf("[CPU] %04X: JR NZ,%d\n", PC, offset);
      if (!checkFlag(ZF)) {
        PC += 2 + offset;
        cycles = 12;
      } else {
        PC += 2;
        cycles = 8;
      }
      break;
    }
    case 0x21: // LD HL, d16
    {
      uint16_t val = read16(PC + 1);
      std::printf("[CPU] %04X: LD HL, 0x%04X\n", PC, val);
      setHL(val);
      PC += 3;
      cycles = 12;
      break;
    }
    case 0x22: // LD (HL+), A
    {
      uint16_t addr = getHL();
      write8(addr, A);
      std::printf("[CPU] %04X: LD (HL+), A ; [0x%04X] = 0x%02X\n", PC, addr, A);
      setHL(addr + 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x23: {
      std::printf("[CPU] %04X: INC HL\n", PC);
      setHL(getHL() + 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x24:
      instr_INC(4);
      break;
    case 0x25:
      instr_DEC(4);
      break;
    case 0x26: { // LD H, imm8
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD H, 0x%02X\n", PC, val);
      H = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x27: { // DAA
      std::printf("[CPU] %04X: DAA\n", PC);
      uint8_t correction = 0;
      bool carry = false;

      if (checkFlag(HF) || (!checkFlag(NF) && (A & 0x0F) > 9)) {
        correction |= 0x06;
      }
      if (checkFlag(CF) || (!checkFlag(NF) && A > 0x99)) {
        correction |= 0x60;
        carry = true;
      }

      A += checkFlag(NF) ? -correction : correction;

      setFlag(F, ZF, A == 0);
      setFlag(F, HF, false);
      setFlag(F, CF, carry);

      PC += 1;
      cycles = 4;
      break;
    }

    case 0x28: { // JR Z,r8
      int8_t offset = static_cast<int8_t>(read8(PC + 1));
      std::printf("[CPU] %04X: JR Z,%d\n", PC, offset);
      if (checkFlag(ZF)) {
        PC += 2 + offset;
        cycles = 8;
      } else {
        PC += 2;
        cycles = 12;
      }
      break;
    }
    case 0x29: // ADD HL, HL
    {
      uint32_t result = getHL() + getHL();
      std::printf("[CPU] %04X: ADD HL, HL\n", PC);

      F &= 0x80;
      if ((getHL() & 0x0FFF) + (getHL() & 0x0FFF) > 0x0FFF)
        F |= 0x20;
      if (result > 0xFFFF)
        F |= 0x10;

      setHL(result & 0xFFFF);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x2A: // LD A, (HL+)
    {
      uint16_t addr = getHL();
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (HL+) ; A = [0x%04X] = 0x%02X\n", PC, addr,
                  A);
      setHL(addr + 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x2B: {
      std::printf("[CPU] %04X: DEC HL\n", PC);
      setHL(getHL() - 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x2C:
      instr_INC(5);
      break;
    case 0x2D:
      instr_DEC(5);
      break;
    case 0x2E: { // LD L, imm8
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD L, 0x%02X\n", PC, val);
      L = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x2F: { // CPL
      std::printf("[CPU] %04X: CPL\n", PC);
      A = ~A;
      setFlag(F, NF, true);
      setFlag(F, HF, true);
      PC += 1;
      cycles = 4;
      break;
    }

    case 0x30: { // JR NC,r8
      int8_t offset = static_cast<int8_t>(read8(PC + 1));
      std::printf("[CPU] %04X: JR NC,%d\n", PC, offset);
      if (!checkFlag(CF)) {
        PC += 2 + offset;
        cycles = 8;
      } else {
        PC += 2;
        cycles = 12;
      }
      break;
    }
    case 0x31: // LD SP, d16
    {
      uint16_t val = read16(PC + 1);
      std::printf("[CPU] %04X: LD SP, 0x%04X\n", PC, val);
      SP = val;
      PC += 3;
      cycles = 12;
      break;
    }
    case 0x32: // LD (HL-), A
    {
      uint16_t addr = getHL();
      write8(addr, A);
      std::printf("[CPU] %04X: LD (HL-), A ; [0x%04X] = 0x%02X\n", PC, addr, A);
      setHL(addr - 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x33: {
      std::printf("[CPU] %04X: INC SP\n", PC);
      SP += 1;
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x34: { // INC (HL)
      uint16_t addr = getHL();
      uint8_t val = read8(addr);
      setFlag(F, HF, (val & 0x0F) == 0x0F); // Half carry from bit 3
      val++;
      setFlag(F, ZF, val == 0);
      setFlag(F, NF, false); // N flag reset
      write8(addr, val);
      PC += 1;
      cycles = 12;
      break;
    }
    case 0x35: { // DEC (HL)
      uint16_t addr = getHL();
      uint8_t val = read8(addr);
      setFlag(F, HF, (val & 0x0F) == 0x00); // Half borrow from bit 4
      val--;
      setFlag(F, ZF, val == 0);
      setFlag(F, NF, true); // N flag set
      write8(addr, val);
      PC += 1;
      cycles = 12;
      break;
    }
    case 0x36: { // LD (HL), n
      uint8_t value = read8(PC + 1);
      uint16_t addr = getHL();
      write8(addr, value);
      std::printf("[CPU] %04X: LD (HL), 0x%02X ; [0x%04X] = 0x%02X\n", PC,
                  value, addr, value);
      PC += 2;
      cycles = 12;
      break;
    }
    case 0x37: { // SCF
      std::printf("[CPU] %04X: SCF\n", PC);
      setFlag(F, CF, true);
      setFlag(F, NF, false);
      setFlag(F, HF, false);
      PC += 1;
      cycles = 4;
      break;
    }

    case 0x38: { // JR C,r8
      int8_t offset = static_cast<int8_t>(read8(PC + 1));
      std::printf("[CPU] %04X: JR C,%d\n", PC, offset);
      if (checkFlag(CF)) {
        PC += 2 + offset;
        cycles = 8;
      } else {
        PC += 2;
        cycles = 12;
      }
      break;
    }
    case 0x39: // ADD HL, SP
    {
      std::printf("[CPU] %04X: ADD HL, SP\n", PC);
      uint32_t result = getHL() + SP;

      F &= 0x10; //  carry flag
      if ((getHL() & 0x0FFF) + (SP & 0x0FFF) > 0x0FFF)
        F |= 0x20; // H
      if (result > 0xFFFF)
        F |= 0x10; // C
      F &= ~0x40;  // N = 0

      setHL(result & 0xFFFF);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x3A: // LD A, (HL-)
    {
      uint16_t addr = getHL();
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (HL-) ; A = [0x%04X] = 0x%02X\n", PC, addr,
                  A);
      setHL(addr - 1);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x3B: {
      std::printf("[CPU] %04X: DEC SP\n", PC);
      SP -= 1;
      PC += 1;
      cycles = 8;
      break;
    }
    case 0x3C:
      instr_INC(7);
      break;
    case 0x3D:
      instr_DEC(7);
      break;
    case 0x3E: // LD A, imm8
    {
      uint8_t val = read8(PC + 1);
      std::printf("[CPU] %04X: LD A, 0x%02X\n", PC, val);
      A = val;
      PC += 2;
      cycles = 8;
      break;
    }
    case 0x3F: { // CCF
      std::printf("[CPU] %04X: CCF\n", PC);
      setFlag(F, CF, !checkFlag(CF));
      setFlag(F, NF, false);
      setFlag(F, HF, false);
      PC += 1;
      cycles = 4;
      break;
    }

      // 0x40 to 0x7f to handler moved to handler

    case 0x80:
      instr_ADD(B);
      PC += 1;
      break;
    case 0x81:
      instr_ADD(C);
      PC += 1;
      break;
    case 0x82:
      instr_ADD(D);
      PC += 1;
      break;
    case 0x83:
      instr_ADD(E);
      PC += 1;
      break;
    case 0x84:
      instr_ADD(H);
      PC += 1;
      break;
    case 0x85:
      instr_ADD(L);
      PC += 1;
      break;
    case 0x86:
      instr_ADD(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0x87:
      instr_ADD(A);
      PC += 1;
      break;

    case 0x88:
      instr_ADC(B);
      PC += 1;
      break;
    case 0x89:
      instr_ADC(C);
      PC += 1;
      break;
    case 0x8A:
      instr_ADC(D);
      PC += 1;
      break;
    case 0x8B:
      instr_ADC(E);
      PC += 1;
      break;
    case 0x8C:
      instr_ADC(H);
      PC += 1;
      break;
    case 0x8D:
      instr_ADC(L);
      PC += 1;
      break;
    case 0x8E:
      instr_ADC(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0x8F:
      instr_ADC(A);
      PC += 1;
      break;

    case 0x90:
      instr_SUB(B);
      PC += 1;
      break;
    case 0x91:
      instr_SUB(C);
      PC += 1;
      break;
    case 0x92:
      instr_SUB(D);
      PC += 1;
      break;
    case 0x93:
      instr_SUB(E);
      PC += 1;
      break;
    case 0x94:
      instr_SUB(H);
      PC += 1;
      break;
    case 0x95:
      instr_SUB(L);
      PC += 1;
      break;
    case 0x96:
      instr_SUB(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0x97:
      instr_SUB(A);
      PC += 1;
      break;

    case 0x98:
      instr_SBC(B);
      PC += 1;
      break;
    case 0x99:
      instr_SBC(C);
      PC += 1;
      break;
    case 0x9A:
      instr_SBC(D);
      PC += 1;
      break;
    case 0x9B:
      instr_SBC(E);
      PC += 1;
      break;
    case 0x9C:
      instr_SBC(H);
      PC += 1;
      break;
    case 0x9D:
      instr_SBC(L);
      PC += 1;
      break;
    case 0x9E:
      instr_SBC(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0x9F:
      instr_SBC(A);
      PC += 1;
      break;

    case 0xA0:
      instr_AND(B);
      PC += 1;
      break;
    case 0xA1:
      instr_AND(C);
      PC += 1;
      break;
    case 0xA2:
      instr_AND(D);
      PC += 1;
      break;
    case 0xA3:
      instr_AND(E);
      PC += 1;
      break;
    case 0xA4:
      instr_AND(H);
      PC += 1;
      break;
    case 0xA5:
      instr_AND(L);
      PC += 1;
      break;
    case 0xA6:
      instr_AND(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0xA7:
      instr_AND(A);
      PC += 1;
      break;

    case 0xA8:
      instr_XOR(B);
      PC += 1;
      break;
    case 0xA9:
      instr_XOR(C);
      PC += 1;
      break;
    case 0xAA:
      instr_XOR(D);
      PC += 1;
      break;
    case 0xAB:
      instr_XOR(E);
      PC += 1;
      break;
    case 0xAC:
      instr_XOR(H);
      PC += 1;
      break;
    case 0xAD:
      instr_XOR(L);
      PC += 1;
      break;
    case 0xAE:
      instr_XOR(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0xAF:
      instr_XOR(A);
      PC += 1;
      break;

    case 0xB0:
      instr_OR(B);
      PC += 1;
      break;
    case 0xB1:
      instr_OR(C);
      PC += 1;
      break;
    case 0xB2:
      instr_OR(D);
      PC += 1;
      break;
    case 0xB3:
      instr_OR(E);
      PC += 1;
      break;
    case 0xB4:
      instr_OR(H);
      PC += 1;
      break;
    case 0xB5:
      instr_OR(L);
      PC += 1;
      break;
    case 0xB6:
      instr_OR(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0xB7:
      instr_OR(A);
      PC += 1;
      break;

    case 0xB8:
      instr_CP(B);
      PC += 1;
      break;
    case 0xB9:
      instr_CP(C);
      PC += 1;
      break;
    case 0xBA:
      instr_CP(D);
      PC += 1;
      break;
    case 0xBB:
      instr_CP(E);
      PC += 1;
      break;
    case 0xBC:
      instr_CP(H);
      PC += 1;
      break;
    case 0xBD:
      instr_CP(L);
      PC += 1;
      break;
    case 0xBE:
      instr_CP(read8(getHL()));
      PC += 1;
      cycles = 8;
      break;
    case 0xBF:
      instr_CP(A);
      PC += 1;
      break;

    case 0xC0: { // RET NZ
      std::printf("[CPU] %04X: RET NZ\n", PC);
      if (!checkFlag(ZF)) {
        PC = pop16();
        cycles = 20;
      } else {
        PC += 1;
        cycles = 8;
      }
      break;
    }
    case 0xC1: // POP BC
    {
      uint16_t val = pop16();
      B = val >> 8;
      C = val & 0xFF;
      std::printf("[CPU] %04X: POP BC\n", PC);
      PC += 1;
      cycles = 12;
      break;
    }
    case 0xC2: { // JP NZ,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: JP NZ,0x%04X\n", PC, addr);

      if (!checkFlag(ZF)) {
        PC = addr;
        cycles = 16;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xC3: // JP a16
    {
      uint16_t addr = read8(PC + 1) | (read8(PC + 2) << 8);
      std::printf("[CPU] %04X: JP 0x%04X\n", PC, addr);
      PC = addr;
      cycles = 16;
      break;
    }
    case 0xC4: { // CALL NZ,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: CALL NZ,0x%04X\n", PC, addr);
      if (!checkFlag(ZF)) {
        push16(PC + 3);
        PC = addr;
        cycles = 24;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xC5: { // PUSH BC
      push16((B << 8) | C);
      std::printf("[CPU] %04X: PUSH BC\n", PC);
      PC += 1;
      cycles = 16;
      break;
    }
    case 0xC6: { // ADD A, n
      uint8_t value = read8(PC + 1);
      instr_ADD(value);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xC7:
      handleVector(opcode);
      break;

    case 0xC8: { // RET Z
      std::printf("[CPU] %04X: RET Z\n", PC);
      if (checkFlag(ZF)) {
        PC = pop16();
        cycles = 20;
      } else {
        PC += 1;
        cycles = 8;
      }

      break;
    }
    case 0xC9: { // RET
      uint16_t addr = pop16();
      std::printf("[CPU] %04X: RET → 0x%04X (SP=%04X)\n", PC, addr, SP);
      PC = addr;
      cycles = 16;
      break;
    }
    case 0xCA: { // JP Z,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: JP Z,0x%04X\n", PC, addr);
      if (checkFlag(ZF)) {
        PC = addr;
        cycles = 16;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xCB: {
      handleCB();
    }
    case 0xCC: { // CALL Z,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: CALL Z,0x%04X\n", PC, addr);
      if (checkFlag(ZF)) {
        push16(PC + 3);
        PC = addr;
        cycles = 24;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xCD: // CALL a16
    {
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: CALL 0x%04X\n", PC, addr);
      push16(PC + 3);
      PC = addr;
      cycles = 24;
      break;
    }
    case 0xCE: { // ADC A, n
      uint8_t value = read8(PC + 1);
      instr_ADC(value);
      PC += 2;
      cycles = 8; // rewrites cycles
      break;
    }
    case 0xCF:
      handleVector(opcode);
      break;

    case 0xD1: // POP DE
    {
      uint16_t val = pop16();
      D = val >> 8;
      E = val & 0xFF;
      std::printf("[CPU] %04X: POP DE\n", PC);
      PC += 1;
      cycles = 12;
      break;
    }
    case 0xD2: { // JP NC,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: JP NC,0x%04X\n", PC, addr);
      if (!checkFlag(CF)) {
        PC = addr;
        cycles = 16;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xD4: { // CALL NC,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: CALL NC,0x%04X\n", PC, addr);
      if (!checkFlag(CF)) {
        push16(PC + 3);
        PC = addr;
        cycles = 24;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xD5: { // PUSH DE
      push16((D << 8) | E);
      std::printf("[CPU] %04X: PUSH DE\n", PC);
      PC += 1;
      cycles = 16;
      break;
    }
    case 0xD6: { // SUB n
      uint8_t value = read8(PC + 1);
      instr_SUB(value);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xD7:
      handleVector(opcode);
      break;

    case 0xD8: { // RET C
      std::printf("[CPU] %04X: RET C\n", PC);
      if (checkFlag(CF)) {
        PC = pop16();
        cycles = 20;
      } else {
        PC += 1;
        cycles = 8;
      }
      break;
    }
    case 0xD9: // RETI
    {
      uint16_t addr = pop16();
      std::printf("[CPU] %04X: RETI to 0x%04X (Enable Interrupts!)\n", PC,
                  addr);
      IME = true; // Re-enable interrupts
      PC = addr;
      cycles = 16;
      break;
    }
    case 0xDA: { // JP C,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: JP C,0x%04X\n", PC, addr);
      if (checkFlag(CF)) {
        PC = addr;
        cycles = 16;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xDC: { // CALL C,a16
      uint16_t addr = read16(PC + 1);
      std::printf("[CPU] %04X: CALL C,0x%04X\n", PC, addr);
      if (checkFlag(CF)) {
        push16(PC + 3);
        PC = addr;
        cycles = 24;
      } else {
        PC += 3;
        cycles = 12;
      }
      break;
    }
    case 0xDE: { // SBC A, n
      uint8_t value = read8(PC + 1);
      instr_SBC(value);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xDF:
      handleVector(opcode);
      break;

    case 0xE0: // LD (a8), A
    {
      uint8_t offset = read8(PC + 1);
      uint16_t addr = 0xFF00 + offset;
      std::printf("[CPU] %04X: LD (0xFF00 + 0x%02X), A ; A=0x%02X\n", PC,
                  offset, A);
      write8(addr, A);
      PC += 2;
      cycles = 12;
      break;
    }
    case 0xE1: // POP HL
    {
      uint16_t val = pop16();
      H = val >> 8;
      L = val & 0xFF;
      std::printf("[CPU] %04X: POP HL\n", PC);
      PC += 1;
      cycles = 12;
      break;
    }
    case 0xE2: // LD (C), A  => writes A to (0xFF00 + C)
    {
      uint16_t addr = 0xFF00 + C;
      std::printf("[CPU] %04X: LD (0xFF00 + C), A ; C=0x%02X A=0x%02X\n", PC, C,
                  A);
      write8(addr, A);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0xE5: { // PUSH HL
      push16((H << 8) | L);
      std::printf("[CPU] %04X: PUSH HL\n", PC);
      PC += 1;
      cycles = 16;
      break;
    }
    case 0xE6: { // AND n
      uint8_t value = read8(PC + 1);
      instr_AND(value);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xE7:
      handleVector(opcode);
      break;

    case 0xE8: { // ADD SP, r8
      int8_t r8 = static_cast<int8_t>(read8(PC + 1));
      uint16_t oldSP = SP;
      SP = SP + r8;

      F = 0; // Zero and Subtract flags cleared
      if (((oldSP & 0x0F) + (r8 & 0x0F)) > 0x0F)
        F |= HF;
      if (((oldSP & 0xFF) + (r8 & 0xFF)) > 0xFF)
        F |= CF;

      PC += 2;
      cycles = 16;
      break;
    }
    case 0xE9: { // JP (HL)
      std::printf("[CPU] %04X: JP (HL) ; Jumping to 0x%04X\n", PC, getHL());
      PC = getHL();
      cycles = 4;
      break;
    }
    case 0xEA: // LD (a16), A
    {
      uint16_t addr = read8(PC + 1) | (read8(PC + 2) << 8);
      std::printf("[CPU] %04X: LD (0x%04X), A\n", PC, addr);
      write8(addr, A);
      PC += 3;
      cycles = 16;
      break;
    }
    case 0xEE: { // XOR n
      uint8_t value = read8(PC + 1);
      A ^= value;
      setFlag(F, ZF, A == 0);
      setFlag(F, NF, false);
      setFlag(F, HF, false);
      setFlag(F, CF, false);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xEF:
      handleVector(opcode);
      break;

    case 0xF0: // LD A, (a8)
    {
      uint8_t offset = read8(PC + 1);
      uint16_t addr = 0xFF00 + offset;
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (0xFF00 + 0x%02X) ; A=0x%02X\n", PC,
                  offset, A);
      PC += 2;
      cycles = 12;
      break;
    }
    case 0xF1: // POP AF
    {
      uint16_t val = pop16();
      A = val >> 8;
      F = val & 0xF0;
      std::printf("[CPU] %04X: POP AF\n", PC);
      PC += 1;
      cycles = 12;
      break;
    }
    case 0xF2: // LD A, (C) => loads A from (0xFF00 + C)
    {
      uint16_t addr = 0xFF00 + C;
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (0xFF00 + C) ; C=0x%02X A=0x%02X\n", PC, C,
                  A);
      PC += 1;
      cycles = 8;
      break;
    }
    case 0xF3: { // DI
      std::printf("[CPU] %04X: DI\n", PC);
      IME = false; // Immediately disable interrupts
      PC += 1;
      cycles = 4;
      break;
    }
    case 0xF5: { // PUSH AF
      push16((A << 8) | F);
      std::printf("[CPU] %04X: PUSH AF\n", PC);
      PC += 1;
      cycles = 16;
      break;
    }
    case 0xF6: { // OR n
      uint8_t value = read8(PC + 1);
      instr_OR(value);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xF7:
      handleVector(opcode);
      break;

    case 0xF8: { // LD HL, SP+r8
      int8_t r8 = static_cast<int8_t>(read8(PC + 1));
      uint16_t result = SP + r8;

      F = 0; // Z=0, N=0
      if (((SP & 0x0F) + (r8 & 0x0F)) > 0x0F)
        F |= HF; // Half-Carry
      if (((SP & 0xFF) + (r8 & 0xFF)) > 0xFF)
        F |= CF; // Carry

      setHL(result);
      PC += 2;
      cycles = 12;
      break;
    }
    case 0xF9: // LD SP, HL
    {
      std::printf("[CPU] %04X: LD SP, HL ; SP=0x%04X\n", PC, getHL());
      SP = getHL();
      PC += 1;
      cycles = 8;
      break;
    }
    case 0xFA: // LD A, (a16)
    {
      uint16_t addr = read8(PC + 1) | (read8(PC + 2) << 8);
      A = read8(addr);
      std::printf("[CPU] %04X: LD A, (0x%04X) ; A = 0x%02X\n", PC, addr, A);
      PC += 3;
      cycles = 16;

      break;
    }
    case 0xFB: { // EI
      std::printf("[CPU] %04X: EI\n", PC);
      pendingEnableIME = true; // Enable interrupts after next instruction
      PC += 1;
      cycles = 4;
      break;
    }
    case 0xFE: { // CP n
      uint8_t value = read8(PC + 1);
      instr_CP(value);
      PC += 2;
      cycles = 8;
      break;
    }
    case 0xFF:
      handleVector(opcode);
      break;

    default: {
      std::cout << "[CPU] Unknown code:" << PC << "(" << opcode << ")\n";
      PC += 1;
      break;
    }
    }
  }
  if (pendingEnableIME) {
    IME = true;
    pendingEnableIME = false;
    std::cout << "[CPU] Enable IME!!!\n";
  }
  return cycles;
}
