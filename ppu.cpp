#include "ppu.h"
#include<iostream>

void PPU::step(int cycles) {
    cycle += cycles;
    if (cycle >= 456) {
        cycle -= 456;
        scanline++;
        if (scanline < 144) {
            renderScanline(scanline);
        }
        if (scanline == 144) {
            // V-Blank interrupt
            // Trigger interrupt here if needed
        }
        if (scanline >= 154) {
            scanline = 0;
        }
    }
}

void PPU::renderScanline(int line) {
    uint8_t lcdc = io[0x40];  // LCDC register
    uint8_t scy = io[0x42];   // Scroll Y
    uint8_t scx = io[0x43];   // Scroll X

    if (!(lcdc & 0x80)) {
        // LCD disabled
        memset(&framebuffer[line * 160], 0, 160);
        return;
    }

    bool bgTileMapSelect = lcdc & 0x08;  // 0x9800 or 0x9C00
    bool bgTileDataSelect = lcdc & 0x10; // 0x8000 or 0x8800 (signed index)

    uint16_t tileMapBase = bgTileMapSelect ? 0x1C00 : 0x1800;
    uint16_t tileDataBase = bgTileDataSelect ? 0x0000 : 0x0800; // VRAM offset

    for (int x = 0; x < 160; x++) {
        int bgX = (x + scx) & 0xFF;
        int bgY = (line + scy) & 0xFF;

        int tileCol = bgX / 8;
        int tileRow = bgY / 8;
        uint16_t mapIndex = tileRow * 32 + tileCol;

        uint8_t tileIndex = vram[tileMapBase + mapIndex];
        int8_t signedIndex = static_cast<int8_t>(tileIndex);
        uint16_t tileAddr = tileDataBase + (bgTileDataSelect ? tileIndex : (signedIndex + 128)) * 16;

        int pixelX = 7 - (bgX % 8);  // Bit 7 is leftmost pixel
        int pixelY = bgY % 8;
        uint8_t byte1 = vram[tileAddr + pixelY * 2];
        uint8_t byte2 = vram[tileAddr + pixelY * 2 + 1];

        int colorBit = ((byte2 >> pixelX) & 1) << 1 | ((byte1 >> pixelX) & 1);
        framebuffer[line * 160 + x] = mapColor(colorBit);
        renderSprites(line);
    }
}

void PPU::renderSprites(int line) {
    uint8_t lcdc = io[0x40];
    if (!(lcdc & 0x02)) return;  // Sprites disabled

    bool objSize = lcdc & 0x04;  // 8x16 mode
    int spriteHeight = objSize ? 16 : 8;

    for (int i = 0; i < 40; i++) {
        uint8_t y = oam[i * 4] - 16;
        uint8_t x = oam[i * 4 + 1] - 8;
        uint8_t tile = oam[i * 4 + 2];
        uint8_t flags = oam[i * 4 + 3];

        if (line < y || line >= y + spriteHeight) continue;

        int row = line - y;
        if (flags & 0x40) row = spriteHeight - 1 - row;  // Y flip

        uint16_t addr = tile * 16 + row * 2;
        uint8_t byte1 = vram[addr];
        uint8_t byte2 = vram[addr + 1];

        for (int col = 0; col < 8; col++) {
            int pixelX = flags & 0x20 ? col : 7 - col;  // X flip
            int colorBit = ((byte2 >> pixelX) & 1) << 1 | ((byte1 >> pixelX) & 1);

            if (colorBit == 0) continue;  // Transparent
            int screenX = x + col;
            if (screenX < 0 || screenX >= 160) continue;

            // Priority: skip if BG is non-zero and OBJ_BG_PRIORITY is set
            if ((flags & 0x80) && framebuffer[line * 160 + screenX] < 255) continue;

            framebuffer[line * 160 + screenX] = mapSpriteColor(colorBit, flags & 0x10);
        }
    }
}

uint8_t PPU::mapColor(int colorID) {
    uint8_t pal = io[0x47];  // BGP
    int shade = (pal >> (colorID * 2)) & 0x03;

    switch (shade) {
    case 0: return 255;
    case 1: return 192;
    case 2: return 96;
    case 3: return 0;
    }
    return 255;
}

uint8_t PPU::mapSpriteColor(int colorID, bool useOBP1) {
    uint8_t pal = io[useOBP1 ? 0x49 : 0x48];  // OBP1 or OBP0
    int shade = (pal >> (colorID * 2)) & 0x03;

    switch (shade) {
    case 0: return 255;
    case 1: return 192;
    case 2: return 96;
    case 3: return 0;
    }
    return 255;
}