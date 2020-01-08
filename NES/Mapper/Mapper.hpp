#pragma once
#include <stdint.h>
#include <vector>
#include "../Common/Typedefs.hpp"

class Mapper {
  protected:
    std::vector<u8> prgCode;

  public:
    Mapper(std::vector<u8> &prgCode): prgCode(prgCode) { }
    virtual ~Mapper() { }
    virtual u8* read(u16 address) = 0;
    virtual void write(u16 address, u8 data) = 0;
};