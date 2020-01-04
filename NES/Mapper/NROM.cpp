#include "NROM.hpp"
#include "../ROM.hpp"

uint8_t* NROM::read(uint16_t address) {
    address = (address - 0x8000) % rom->getPrgCode().size();
    return &rom->getPrgCode()[address];
}

void NROM::write(uint16_t address, uint8_t data) {
 //No write in NROM
}