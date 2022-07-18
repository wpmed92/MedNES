#pragma once
#include <stdint.h>

#include <vector>

#include "../Common/Typedefs.hpp"

namespace MedNES {

class Mapper {
   public:
    Mapper(std::vector<u8> &prgCode, std::vector<u8> &chrROM, int mirroring) : prgCode(prgCode),
                                                                               chrROM(chrROM),
                                                                               mirroring(mirroring) {}

    virtual ~Mapper() {}
    virtual u8 read(u16 address) = 0;
    virtual void write(u16 address, u8 data) = 0;
    virtual u8 ppuread(u16 address);
    virtual void ppuwrite(u16 address, u8 data);
    int getMirroring() { return mirroring; }

   protected:
    std::vector<u8> prgCode;
    std::vector<u8> chrROM;
    int mirroring;
};

};  //namespace MedNES
