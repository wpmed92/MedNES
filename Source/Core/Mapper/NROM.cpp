#include "NROM.hpp"

namespace MedNES {

u8 NROM::read(u16 address) {
    if (address < 0x8000) {
        return 0;
    }

    address = (address - 0x8000) % prgCode.size();
    return prgCode[address];
}

void NROM::write(u16 address, u8 data) {
    //No write in NROM
}

}  //namespace MedNES
