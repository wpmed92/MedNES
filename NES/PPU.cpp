#include "PPU.hpp"
#include <iostream>
#include <bitset>

void PPU::tick() {
    if (scanLine == -1) { //pre-render scanline
        
    } else if (scanLine >= 0 && scanLine <= 239) {
        
    } else if (scanLine == 240) { //pos-render scanline
        
    } else if (scanLine >= 241 && scanLine <= 260) { //vblank
        
    }
    
    if (dot == 341) {
        scanLine++;
        dot = 0;
    } else {
        dot++;
    }
}

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
    if (address == 0x4014) {
        oamdma = data;
    } else {
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
}

void PPU::copyOAM(uint8_t oamEntry, int index) {
    int oamSelect = index / 4;
    int property = index % 4;
    
    if (property == 0) {
        primaryOAM[oamSelect].spriteY = oamEntry;
    } else if (property == 1) {
        primaryOAM[oamSelect].spriteTileNum = oamEntry;
    } else if (property == 2) {
        primaryOAM[oamSelect].spriteAttr = oamEntry;
    } else {
        primaryOAM[oamSelect].spriteX = oamEntry;
    }
}

void PPU::drawPatternTable() {
    std::vector<uint8_t> chrData = rom->getChrData();
    
    for (int i = 0; i < chrData.size(); i++) {
        std::bitset<8> x1(chrData.at(i));
        std::cout << x1 << "\n";
    }
}
