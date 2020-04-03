#include "UnROM.hpp"

u8* UnROM::read(u16 address) {
    u32 address_32 = 0;

    if (address >= 0xC000) {
        address_32 = lastBankStart + (address - 0xC000);
    } else {
        address_32 = (address - 0x8000) + bankSelect * 16384;
    }

    return &prgCode[address_32];
}

void UnROM::write(u16 address, u8 data) {
 bankSelect = data & 7;
}