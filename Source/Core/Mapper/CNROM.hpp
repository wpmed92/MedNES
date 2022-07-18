#pragma once

#include "Mapper.hpp"

namespace MedNES {

class CNROM : public Mapper {
   public:
    CNROM(std::vector<u8> &prgCode, std::vector<u8> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {}
    ~CNROM() override = default;
    u8 read(u16 address) override;
    void write(u16 address, u8 data) override;
    u8 ppuread(u16 address) override;
    void ppuwrite(u16 address, u8 data) override;

   private:
    u8 bankSelect = 0;
};

};  //namespace MedNES
