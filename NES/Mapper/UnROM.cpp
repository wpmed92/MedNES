#include "UnROM.hpp"

uint8_t* UnROM::read(uint16_t address) {
    uint32_t address_32 = 0;

    if (address >= 0xC000) {
        address_32 = lastBankStart + (address - 0xC000);
    } else {
        address_32 = (address - 0x8000) + bankSelect * 16384;
    }

    return &prgCode[address_32];
}

void UnROM::write(uint16_t address, uint8_t data) {
 bankSelect = data & 7;
}