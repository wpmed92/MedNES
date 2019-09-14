//
//  PPU.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 21..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#include "PPU.hpp"

uint8_t* PPU::read(uint16_t address) {
    address %= 8;
    
    if (address == 0) {
        return &ppuctrl;
    } else if (address == 1) {
        return &ppumask;
    } else if (address == 2) {
        return &ppustatus;
    } else if (address == 3) {
        return &oamaddr;
    } else if (address == 4) {
        return &oamdata;
    } else if (address == 5) {
        return &ppuscroll;
    } else if (address == 6) {
        return &ppuaddr;
    } else if (address == 7) {
        return &ppudata;
    }
    
    return nullptr;
}

void PPU::write(uint16_t address, uint8_t data) {
    address %= 8;
    
    if (address == 0) {
        ppuctrl = data;
    } else if (address == 1) {
        ppumask = data;
    } else if (address == 2) {
        ppustatus = data;
    } else if (address == 3) {
        oamaddr = data;
    } else if (address == 4) {
        oamdata = data;
    } else if (address == 5) {
        ppuscroll = data;
    } else if (address == 6) {
        ppuaddr = data;
    } else if (address == 7) {
        ppudata = data;
    }
}

void drawPatternTable() {
    
}
