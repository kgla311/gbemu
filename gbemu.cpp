#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <iostream>

#include "emu.h"
#include "ppu.h"

int main(int argc, char **args) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("Game Boy Emu", 100, 100, 160, 144, 0);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, 160, 144);

  Emulator emu;
  PPU ppu;

  bool running = true;
  SDL_Event event;

  while (running) {

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = false;

      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        std::cout << "SDL saw key move: "
                  << SDL_GetKeyName(event.key.keysym.sym) << "\n";
        bool pressed = (event.type == SDL_KEYDOWN);
        switch (event.key.keysym.sym) {
        case SDLK_d:
          emu.setButton(0, pressed);
          break; // Right
        case SDLK_a:
          emu.setButton(1, pressed);
          break; // Left
        case SDLK_w:
          emu.setButton(2, pressed);
          break; // Up
        case SDLK_s:
          emu.setButton(3, pressed);
          break; // Down
        case SDLK_k:
          emu.setButton(4, pressed);
          break; // A
        case SDLK_j:
          emu.setButton(5, pressed);
          break; // B
        case SDLK_RSHIFT:
          emu.setButton(6, pressed);
          break; // Select
        case SDLK_RETURN:
          emu.setButton(7, pressed);
          break; // Start
        }
      }
    }

    // Now render based on state:
    if (emu.isButtonPressed(2)) {                           // Up
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
    } else {
      SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255); // green
    }

    emu.tick();
    emu.runFrame();

    SDL_UpdateTexture(texture, nullptr, emu.getFramebuffer().data(), 160 * 4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    ppu.io = emu.IO; // gets acces to memory

    ppu.step(emu.cycles);
    if (ppu.scanline == 144 && ppu.cycle < emu.cycles) { // Frame is done
      emu.renderFrame(renderer, ppu.framebuffer);
    }
  }
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
