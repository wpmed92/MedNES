#pragma once

#include "Mapper.hpp"

class NROM : public Mapper {
  public:
    NROM(std::vector<uint8_t> prgCode) : Mapper(prgCode) { }
    ~NROM() override = default;
    uint8_t* read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;
};