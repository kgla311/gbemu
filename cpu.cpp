#include "cpu.h"
#include <cstdio>
#include <iostream>

CPU::CPU(MMU &mmu_) : mmu(mmu_) { reset(); }

// #########################################################################################################################################################################
//  junk
// #########################################################################################################################################################################

void CPU::reset() {
  A = 0x01;
  F = 0xB0;
  B = 0x00;
  C = 0x13;
  D = 0x00;
  E = 0xD8;
  H = 0x01;
  L = 0x4D;
  SP = 0xFFFE;
  PC = 0x0100;

  cycles = 0;
}

void CPU::checkInterrupts() {
  uint8_t IE = mmu.read8(0xFFFF); // Interrupt Enable
  uint8_t IF = mmu.read8(0xFF0F); // Interrupt Flag
  if (!(IE & IF) || !IME)
    return; // No interrupt or IME disabled

  if (halted && ((IE & IF) || IME))
    halted = false;

  for (int i = 0; i < 5; i++) { // 5 possible interrupts
    if ((IE & IF) & (1 << i)) {
      IME = false;
      mmu.write8(0xFF0F, IF & ~(1 << i)); // Clear interrupt flag
      push16(PC);                         // Save current PC
      switch (i) {
      case 0:
        PC = 0x40;
        break; // V-Blank
      case 1:
        PC = 0x48;
        break; // LCD STAT
      case 2:
        PC = 0x50;
        break; // Timer
      case 3:
        PC = 0x58;
        break; // Serial
      case 4:
        PC = 0x60;
        break; // Joypad
      }
      cycles += 20; // Interrupt handling takes 20 cycles
      break;
    }
  }
}

int CPU::step() {
  if (halted) {
    checkInterrupts();
    return 4; // Or appropriate idle cycles (commonly 4)
  }

  uint8_t opcode = read8(PC);
  int cycles = execute(opcode);
  checkInterrupts();
  return cycles;
}

// BUS

uint8_t CPU::read8(uint16_t addr) { return mmu.read8(addr); }

uint16_t CPU::read16(uint16_t addr) { return mmu.read16(addr); }

void CPU::write8(uint16_t addr, uint8_t val) { mmu.write8(addr, val); }

void CPU::write16(uint16_t addr, uint16_t val) { mmu.write16(addr, val); }

// STACK

void CPU::push16(uint16_t val) {
  SP -= 2;
  write16(SP, val);
}

uint16_t CPU::pop16() {
  uint16_t val = read16(SP);
  SP += 2;
  return val;
}

// #########################################################################################################################################################################
//  opcodes moved to cpu_i.cpp file hard to understand but it is working so do
//  not touch unless understand what is going on
// #########################################################################################################################################################################
