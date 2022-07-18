#pragma once

#include "Mapper.hpp"

namespace MedNES {

class NROM : public Mapper {
   public:
    NROM(std::vector<u8> &prgCode, std::vector<u8> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {}
    ~NROM() override = default;
    u8 read(u16 address) override;
    void write(u16 address, u8 data) override;
};

};  //namespace MedNES
