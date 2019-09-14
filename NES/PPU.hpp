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
#include "RAM.hpp"

class PPU : public INESBus {
private:
    uint8_t ppuctrl; //$2000
    uint8_t ppumask; //$2001
    uint8_t ppustatus; //$2002
    uint8_t oamaddr; //$2003
    uint8_t oamdata; //$2004
    uint8_t ppuscroll; //$2005
    uint8_t ppuaddr; //$2006
    uint8_t ppudata; //$2007
    uint8_t oamdma; //$4014
    RAM vram;

public:
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
};

#endif /* PPU_hpp */
