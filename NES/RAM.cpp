//
//  RAM.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 12..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#include "RAM.hpp"

uint8_t* RAM::read(uint16_t address) {
    address %= 2048;
    return &ram[address];
}

void RAM::write(uint16_t address, uint8_t data) {
    address %= 2048;
    ram[address] = data;
}
