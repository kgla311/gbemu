#include "mmu.h"
#include <bits/types/error_t.h>
#include <cstdint>
#include <fstream>
#include <iostream>

MMU::MMU() {
  memory.fill(0);

  TIMA = 0;
  TMA = 0;
  TAC = 0;
  DIV = 0;
  divCounter = 0;
  timerCounter = 0; // Reset timers
}

uint8_t MMU::read8(uint16_t addr) {
  switch (addr) {
  case 0xFF00:
    return input.read();
  }
  return (uint8_t)memory[addr];
}

void MMU::write8(uint16_t addr, uint8_t value) {

  switch (addr) {
  case 0xFF00: // Joypad
    input.write(value);
    break;
  }

  memory[addr] = value;
}

uint16_t MMU::read16(uint16_t addr) {
  uint8_t low = read8(addr);
  uint8_t high = read8(addr + 1);
  return (high << 8) | low;
}

void MMU::write16(uint16_t addr, uint16_t value) {
  write8(addr, value & 0xFF);     // Low byte
  write8(addr + 1, (value >> 8)); // High byte
}

void MMU::requestInterrupt(int id) {
  uint8_t IF = read8(0xFF0F);
  IF |= (1 << id);
  write8(0xFF0F, IF);
}

void MMU::updateTimers(int cycles) {
  // Update DIV register
  divCounter += cycles;
  if (divCounter >= 256) { // DIV increments every 256 cycles
    DIV++;
    divCounter -= 256;
  }

  // Check if timer is enabled (TAC bit 2)
  if (TAC & 0x04) {
    // Determine timer frequency
    int timerFreq = 1024; // default 4096 Hz
    switch (TAC & 0x03) {
    case 0:
      timerFreq = 1024;
      break; // 4096 Hz
    case 1:
      timerFreq = 16;
      break; // 262144 Hz
    case 2:
      timerFreq = 64;
      break; // 65536 Hz
    case 3:
      timerFreq = 256;
      break; // 16384 Hz
    }

    timerCounter += cycles;
    while (timerCounter >= timerFreq) {
      timerCounter -= timerFreq;
      TIMA++;
      if (TIMA == 0) {
        TIMA = TMA;
        requestInterrupt(2); // Timer interrupt (bit 2)
      }
    }
  }
}

void MMU::loadROM(const std::string &path) {

  FILE *fp = fopen("cpu_instrs.gb", "rb");
  if (!fp) {
    perror("Failed to open ROM");
  }

  // 2. Determine file size
  std::fseek(fp, 0, SEEK_END);
  long size = std::ftell(fp);
  std::rewind(fp);

  if (size <= 0) {
    std::fprintf(stderr, "[MMU] ROM is empty or invalid size: %ld\n", size);
    std::fclose(fp);
    std::exit(1);
  }

  // 3. Clear memory, then read up to 0x8000 bytes
  memory.fill(0);
  size_t to_read = static_cast<size_t>(std::min<long>(size, 0x8000));
  size_t read = std::fread(&memory[0], 1, to_read, fp);
  std::fclose(fp);

  if (read != to_read) {
    std::fprintf(stderr, "[MMU] Only read %zu of %zu bytes\n", read, to_read);
    std::exit(1);
  }

  std::printf("[MMU] ROM loaded: %zu bytes\n", read);
}
