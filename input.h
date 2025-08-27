#include <cstdint>
#include <iostream>

class Input {
public:
  uint8_t select = 0x30;
  bool buttons[8] = {};

  void write(uint8_t value);
  uint8_t read() const;
  void press(int id) {
    buttons[id] = true;
    std::cout << "Pressed button's id == " << id << "\n";
  }
  void release(int id) { buttons[id] = false; }

  bool getState(int id) { return buttons[id]; }
};
