
#include "CNROM.hpp"

namespace MedNES {

u8 CNROM::read(u16 address) {
    if (address < 0x8000) {
        return 0;
    }

    address = (address - 0x8000) % prgCode.size();
    return prgCode[address];
}

void CNROM::write(u16 address, u8 data) {
    if (address < 0x8000) {
        return;
    }

    bankSelect = data & 3;
}

u8 CNROM::ppuread(u16 address) {
    u16 ppuaddr = bankSelect * 8192 + address;
    return chrROM[ppuaddr];
}

void CNROM::ppuwrite(u16 address, u8 data) {
}

}  //namespace MedNES
