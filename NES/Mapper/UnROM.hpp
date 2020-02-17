#pragma once

#include "Mapper.hpp"

class UnROM : public Mapper {
  private:
    u8 bankSelect = 0;
    u32 lastBankStart = 0;

  public:
    UnROM(std::vector<u8> &prgCode, std::vector<u8> &chrROM) : Mapper(prgCode, chrROM) { 
      lastBankStart = prgCode.size() - 16384;
    }
    ~UnROM() override = default;
    u8* read(u16 address) override;
    void write(u16 address, u8 data) override;
};