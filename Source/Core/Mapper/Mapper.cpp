#include "Mapper.hpp"

namespace MedNES {

u8 Mapper::ppuread(u16 address) {
    return chrROM[address];
}

void Mapper::ppuwrite(u16 address, u8 data) {
    chrROM[address] = data;
}

}  //namespace MedNES
