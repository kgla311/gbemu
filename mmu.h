#pragma once
#include "input.h"
#include <array>
#include <cstdint>
#include <string>

class MMU {
public:
  Input input;
  MMU();

  uint8_t read8(uint16_t addr);
  void write8(uint16_t addr, uint8_t value);

  uint16_t read16(uint16_t addr);
  void write16(uint16_t addr, uint16_t value);

  void requestInterrupt(int id);

  void updateTimers(int cycles);

  void loadROM(const std::string &path);

  void setButtonState(int button, bool pressed) {
    bool old = input.getState(button);
    if (pressed) {
      input.press(button);
    } else {
      input.release(button);
    }
    if (!old && pressed) {
      requestInterrupt(4); // Joypad interrupt
    }
  }

  bool isButtonPressed(int button) {
    if (button < 0 || button > 8)
      return 0;
    else {
      return input.getState(button);
    }
  }

  std::array<uint8_t, 0x10000> memory; // 64KB address space

  uint8_t *getIO() { return &memory[0xFF00]; } // for ppu

  uint8_t joyp = 0xFF; // handling input
private:
  // Timer registers
  uint8_t TIMA = 0; // Timer counter
  uint8_t TMA = 0;  // Timer modulo
  uint8_t TAC = 0;  // Timer control
  uint16_t DIV = 0; // Divider register

  // Internal cycle counter for timers
  uint16_t divCounter = 0;
  uint16_t timerCounter = 0;
};
