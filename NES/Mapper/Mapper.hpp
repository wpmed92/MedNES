#pragma once
#include <stdint.h>
#include "../ROM.hpp"

class Mapper {
  protected:
    std::vector<uint8_t> prgCode;

  public:
    Mapper(std::vector<uint8_t> &prgCode): prgCode(prgCode) { }
    virtual ~Mapper() { }
    virtual uint8_t* read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t data) = 0;
};