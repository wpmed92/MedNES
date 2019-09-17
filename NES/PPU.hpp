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
#include "ROM.hpp"

struct OAM {
    uint8_t spriteY;
    uint8_t spriteTileNum;
    uint8_t spriteAttr;
    uint8_t spriteX;
};

class PPU : public INESBus {
private:
    //Registers
    uint8_t ppuctrl; //$2000
    uint8_t ppumask; //$2001
    uint8_t ppustatus; //$2002
    uint8_t oamaddr; //$2003
    uint8_t oamdata; //$2004
    uint8_t ppuscroll; //$2005
    uint8_t ppuaddr; //$2006
    uint8_t ppudata; //$2007
    
    //Nametable vram
    uint8_t vram[2048];
    
    //Object Attribute Memory
    uint8_t oamdma; //$4014
    
    OAM primaryOAM[64];
    
    OAM secondaryOAM[8];
    
    ROM* rom;
    
public:
    PPU(ROM* rom) : rom(rom) { }
    void drawPatternTable();
    uint8_t* read(uint16_t address);
    void copyOAM(uint8_t, int);
    void write(uint16_t address, uint8_t data);
};

#endif /* PPU_hpp */
