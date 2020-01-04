#pragma once

#include "Mapper.hpp"

class NROM : public Mapper {
  private:
     std::vector<uint8_t> prgCode;

  public:
    NROM(ROM* rom) : Mapper(rom) { }
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
};