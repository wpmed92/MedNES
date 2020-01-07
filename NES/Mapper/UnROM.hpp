#pragma once

#include "Mapper.hpp"

class UnROM : public Mapper {
  private:
    uint8_t bankSelect = 0;
    uint32_t lastBankStart = 0;

  public:
    UnROM(std::vector<uint8_t> &prgCode) : Mapper(prgCode) { 
      lastBankStart = prgCode.size() - 16384;
    }
    ~UnROM() override = default;
    uint8_t* read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;
};