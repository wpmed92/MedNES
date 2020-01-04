#pragma once

#include "../INESBus.hpp"
class ROM;

class Mapper {
  protected:
    ROM *rom;

  public:
    Mapper(ROM* rom): rom(rom) { }
    virtual ~Mapper() { }
    virtual uint8_t* read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t data) = 0;
};