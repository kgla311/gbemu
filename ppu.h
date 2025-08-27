#include <SDL2/SDL.h>

class PPU {
public:
  uint8_t framebuffer[160 * 144];
  uint8_t vram[0x2000]; // Video RAM
  uint8_t oam[0xA0];    // Sprite Attribute Table (OAM)
  uint8_t *io;          // Pointer to IO registers (LCDC, STAT, etc.)

  int scanline = 0;
  int cycle = 0;

  void step(int cycles);

  void renderScanline(int line);

  void renderSprites(int line);

  uint8_t mapColor(int colorID);
  uint8_t mapSpriteColor(int colorID, bool useOBP1); // use obp1 or not ??
private:
};
