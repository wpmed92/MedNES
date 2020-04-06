#pragma once
#include <stdint.h>
#include <vector>
#include "../Common/Typedefs.hpp"

class Mapper {
  protected:
    std::vector<u8> prgCode;
    std::vector<u8> chrROM;
    
  public:
    Mapper(std::vector<u8> &prgCode, std::vector<u8> &chrROM): prgCode(prgCode), chrROM(chrROM) { }
    virtual ~Mapper() { }
    virtual u8 read(u16 address) = 0;
    virtual void write(u16 address, u8 data) = 0;
    virtual u8 ppuread(u16 address);
    virtual void ppuwrite(u16 address, u8 data);
};