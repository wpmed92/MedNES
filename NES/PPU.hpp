//
//  PPU.hpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 21..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#ifndef PPU_hpp
#define PPU_hpp

#include <stdio.h>
#include <stdint.h>
#include "INESBus.hpp"

class PPU : public INESBus {
private:
    uint8_t ppuctrl;
    uint8_t ppumask;
    uint8_t ppustatus;
    uint8_t oamaddr;
    uint8_t oamdata;
    uint8_t ppuscroll;
    uint8_t ppuaddr;
    uint8_t ppudata;
    uint8_t oamdma;

public:
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
};

#endif /* PPU_hpp */
