#include "NROM.hpp"

u8* NROM::read(u16 address) {
    address = (address - 0x8000) % prgCode.size();
    return &prgCode[address];
}

void NROM::write(u16 address, u8 data) {
 //No write in NROM
}