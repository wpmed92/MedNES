#pragma once

#include "Mapper.hpp"

class UnROM : public Mapper {
  public:
    UnROM(std::vector<uint8_t> &prgCode) : Mapper(prgCode) { }
    ~UnROM() override = default;
    uint8_t* read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;
};