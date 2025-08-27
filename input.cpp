#include "input.h"
#include <cstdint>
#include <iostream>

void Input::write(uint8_t value) {
  // unionly bits 4 and 5 are writable
  select = value & 0x30;
}

uint8_t Input::read() const {
  uint8_t res = 0xCF; // 1100 1111: upper bits set, lower inputs default high

  // Direction keys (bit 4 = 0 means selected)
  if (!(select & 0x10)) {
    if (buttons[0])
      res &= ~0x01; // Right
    if (buttons[1])
      res &= ~0x02; // Left
    if (buttons[2])
      res &= ~0x04; // Up
    if (buttons[3])
      res &= ~0x08; // Down
  }

  // Button keys (bit 5 = 0 means selected)
  if (!(select & 0x20)) {
    if (buttons[4])
      res &= ~0x01; // A
    if (buttons[5])
      res &= ~0x02; // B
    if (buttons[6])
      res &= ~0x04; // Select
    if (buttons[7])
      res &= ~0x08; // Start
  }

  // Merge select bits back in
  return res | (select & 0x30);
}
