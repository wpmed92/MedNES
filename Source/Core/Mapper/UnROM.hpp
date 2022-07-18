#pragma once

#include "Mapper.hpp"

namespace MedNES {

class UnROM : public Mapper {
   public:
    UnROM(std::vector<u8> &prgCode, std::vector<u8> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {
        lastBankStart = prgCode.size() - 16384;
    }
    ~UnROM() override = default;
    u8 read(u16 address) override;
    void write(u16 address, u8 data) override;

   private:
    u8 bankSelect = 0;
    u32 lastBankStart = 0;
};

};  //namespace MedNES
