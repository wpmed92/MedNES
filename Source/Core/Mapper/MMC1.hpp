#pragma once

#include "Mapper.hpp"

namespace MedNES {

class MMC1 : public Mapper {
   public:
    MMC1(std::vector<u8> &prgCode, std::vector<u8> &chrROM, int mirroring) : Mapper(prgCode, chrROM, mirroring) {
        controlReg.val = 0xF;
    }

    ~MMC1() override = default;
    void write(u16 address, u8 data) override;
    u8 read(u16 address) override;
    void ppuwrite(u16 address, u8 data) override;
    u8 ppuread(u16 address) override;

   private:
    //written by CPU
    u8 mmc1SR = 0x10;
    u8 prgRam[0x2000] = {0};

    //internal
    union {
        struct
        {
            unsigned mirroring : 2;
            unsigned prgRomBankMode : 2;
            unsigned chrRomBankMode : 1;
            unsigned padding : 3;
        };

        u8 val;
    } controlReg;

    u8 chrBank0;
    u8 chrBank1;
    u8 prgBank;
};

};  //namespace MedNES
