#pragma once

#include "Mapper.hpp"

class NROM : public Mapper {
  public:
    NROM(std::vector<u8> &prgCode, std::vector<u8> &chrROM) : Mapper(prgCode, chrROM) { }
    ~NROM() override = default;
    u8* read(u16 address) override;
    void write(u16 address, u8 data) override;
};