#include "emu.h"
#include <cstdint>
#include <cstring>
#include <iostream>

Emulator::Emulator() : cpu(mmu) {
  std::cout << "[EMULATOR] Constructor running!\n";
  cycles = 0;
  mmu.loadROM("cpu_instrs.gb");

  framebuffer.fill(0xFF000000);

  std::cout << "[EMULATOR]STOP! Emulator constructor exit!\n";
}

void Emulator::tick() {
  cycles += cpu.step();
  div_counter += cycles * 4;
  while (div_counter >= 256) {
    div_counter -= 256;
    uint8_t div = mmu.read8(0xFF04);
    mmu.write8(0xFF04, div + 1);
  }
}

void Emulator::runFrame() {

  const int CYCLES_PER_FRAME = 4194304 / 60;
  int cycles = 0;
  while (cycles < CYCLES_PER_FRAME) {
    int instruction_cycles = cpu.step();  // Return cycles per instruction
    mmu.updateTimers(instruction_cycles); // Advance hardware timers
    cycles += instruction_cycles;
  }
}

void Emulator::renderFrame(SDL_Renderer *renderer, uint8_t *framebuffer) {
  SDL_Texture *texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 144);

  uint8_t pixels[160 * 144 * 3];
  for (int i = 0; i < 160 * 144; ++i) {
    uint8_t c = framebuffer[i];
    pixels[i * 3 + 0] = c;
    pixels[i * 3 + 1] = c;
    pixels[i * 3 + 2] = c;
  }

  SDL_UpdateTexture(texture, NULL, pixels, 160 * 3);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
  SDL_DestroyTexture(texture);
}

const std::array<uint32_t, 160 * 144> &Emulator::getFramebuffer() const {
  return framebuffer;
}
