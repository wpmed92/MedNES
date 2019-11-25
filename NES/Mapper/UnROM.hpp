#pragma once

#include "Mapper.hpp"

class UnROM : public Mapper {
  private:
    uint8_t bankSelect;

  public:
    UnROM(ROM* rom) : Mapper(rom) { }
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
};