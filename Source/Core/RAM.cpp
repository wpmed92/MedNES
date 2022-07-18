#include "RAM.hpp"

namespace MedNES {

u8 RAM::read(u16 address) {
    address %= 2048;
    return ram[address];
}

void RAM::write(u16 address, u8 data) {
    address %= 2048;
    ram[address] = data;
}

}  //namespace MedNES
