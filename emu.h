#pragma once
#include "cpu.h"
#include "mmu.h"
#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

class Emulator {
public:
  CPU cpu;
  MMU mmu;
  uint16_t cycles;
  uint16_t div_counter = 0;
  uint8_t *IO = mmu.getIO(); // for ppu

  Emulator();

  void tick();     // single CPU cycle
  void runFrame(); // enough cycles for one frame

  void renderFrame(SDL_Renderer *renderer, uint8_t *framebuffer);

  const std::array<uint32_t, 160 * 144> &getFramebuffer() const;

  void setButton(int button, bool pressed) {
    mmu.setButtonState(button, pressed);
  }
  bool isButtonPressed(int button) { return mmu.isButtonPressed(button); }

private:
  std::array<uint32_t, 160 * 144> framebuffer;
  uint8_t getjoypadState() {
    return mmu.joyp;
  }; // 1 = released, 0 = pressed (bitwise) handling input;
};
