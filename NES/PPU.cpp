#include "PPU.hpp"
#include <iostream>
#include <bitset>

void PPU::tick() {
    if (scanLine == 261) {
        scanLine = -1;
        generateFrame = true;
    }
    
    if (scanLine == -1) { //pre-render scanline
        if (dot == 1) {
            ppustatus &= ~0x80;
        }

        pixelIndex = 0;
        //fetch first two tiles
        if (dot >= 321 && dot <= 336) {
            //fetchTiles();
            
            if (dot == 329) {
                //xIncrement();
                shiftReg1 += patternlow;
                shiftReg2 += patternhigh;
            }
            
            if (dot == 336) {
                //xIncrement();
                shiftReg1 += patternlow * 256;
                shiftReg2 += patternhigh * 256;
            }
        }
    } else if (scanLine >= 0 && scanLine <= 239) { //visible scanline
        if (dot >= 1 & dot <= 256) { //start at tile 3
            //emitPixel();
            //fetchTiles();
            
            if (dot > 1 && (dot % 8) == 1) {
                //xIncrement();
                //loadRegisters();
            }
            
            if (dot == 256) {
                //yIncrement();
                shiftReg1 = 0;
                shiftReg2 = 0;
            }
        } else if (dot >= 321 && dot <= 336) { //fetch first two tiles
            //fetchTiles();
            
            if (dot == 329) {
                //xIncrement();
                shiftReg1 += patternlow;
                shiftReg2 += patternhigh;
            }
            
            if (dot == 336) {
                //xIncrement();
                shiftReg1 += patternlow * 256;
                shiftReg2 += patternhigh * 256;
            }
        }
    } else if (scanLine == 240) { //post-render scanline
    } else if (scanLine >= 241 && scanLine <= 260) { //vblank
        if (scanLine == 241 && dot == 1) {
            ppustatus |= 0x80;
        }
    }
    
    if (dot == 341) {
        scanLine++;
        dot = 0;
    } else {
        dot++;
    }
    
}

inline void PPU::xIncrement() {
    if ((v & 0x001F) == 31) {
        v &= ~0x001F;
        v ^= 0x0400;
    } else {
        v += 1;
    }
}

inline void PPU::yIncrement() {
    if ((v & 0x7000) != 0x7000) {
        v += 0x1000;
    } else {
        v &= ~0x7000;
        int y = (v & 0x03E0) >> 5;
        
        if (y == 29) {
            y = 0;
            v ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y += 1;
        }
        
        v = (v & ~0x03E0) | (y << 5);
  }
}

inline void PPU::loadRegisters() {
    shiftReg1 += patternlow * 256;
    shiftReg2 += patternhigh * 256;
}

inline void PPU::fetchTiles() {
    if (dot % 8 == 1) {
        ntbyte = ppuread(0x2000 | (v & 0x0FFF));
    }
    
    if (dot % 8 == 3) {
        attrbyte = 0;
    }
    
    if (dot % 8 == 5) {
        uint16_t patterAddr =
        (((uint16_t) ppuctrl & 0x10) << 7) +
        ((uint16_t) ntbyte << 4) +
        (v & 0x7000);
        patternlow = ppuread(patterAddr);
    }
    
    if (dot % 8 == 7) {
        uint16_t patterAddr =
        (((uint16_t) ppuctrl & 0x10) << 7) +
         ((uint16_t) ntbyte << 4) +
         (v & 0x7000) + 8;
        patternhigh = ppuread(patterAddr);
    }
}

inline void PPU::emitPixel() {
    uint8_t pixel1 = shiftReg1 & 1;
    uint8_t pixel2 = shiftReg2 & 1;
    shiftReg1 >>= 1;
    shiftReg2 >>= 1;
    //frame[pixelIndex++] = (pixel2 << 1) + pixel1;
}

void PPU::printNametable() {
    
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 32; j++) {
            int index = i * 32 + j;
            uint8_t nbyte = ppuread(0x2000 | index);
            /*uint16_t lo = ((ppuctrl & 16) << 8) | ((uint16_t) nbyte << 4);
            uint16_t high = ((ppuctrl & 16) << 8) | ((uint16_t) nbyte << 4) + 8;*/
            
            for (int k = 0; k < 8; k++) {
                /*lo += k;
                high += k;
                uint8_t sliverlo = ppuread(lo);
                uint8_t sliverhigh = ppuread(high);*/
                
                for (int l = 0; l < 8; l++) {
                    /*uint8_t pixello = sliverlo & 128;
                    uint8_t pixelhi = sliverhigh & 128;
                    uint8_t pixel = pixello >> 8 | pixelhi >> 7;*/
                    frame[(i*8+k)*256+(j*8+l)] = nbyte;
                    /*sliverlo >>= 1;
                    sliverhigh >>= 1;*/
                }
            }
        }
    }
}

bool PPU::genNMI() { return (ppuctrl & 0x80) && (*read(0x2002) & 0x80); }

uint8_t* PPU::read(uint16_t address) {
    address %= 8;
    
    if (address == 0) {
        return &ppuctrl;
    } else if (address == 1) {
        return &ppumask;
    } else if (address == 2) {
        ppustatus_cpy = ppustatus;
        ppustatus &= ~0x80;
        w = 0;
        return &ppustatus_cpy;
    } else if (address == 3) {
        return &oamaddr;
    } else if (address == 4) {
        return &oamdata;
    } else if (address == 5) {
        return &ppuscroll;
    } else if (address == 6) {
        return &ppuaddr;
    } else if (address == 7) {
        v += ((ppuctrl & 4) ? 32 : 1);
        v %= 16384;
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
            t = 0;
            t += (data & 0x3) << 10;
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
            if (w == 0) {
                t += (data >> 3);
                x = data & 7;
                w = 1;
            } else {
                t += (data & 7) << 12;
                t += (data & 0xF8) << 2;
                v = t;
                w = 0;
            }
            
            ppuscroll = data;
        } else if (address == 6) {
            if (w == 0) {
                t = 0;
                t = data << 8;
                w = 1;
            } else {
                t += data;
                v = t;
                w = 0;
            }
            
            ppuaddr = data;
        } else if (address == 7) {
            ppuwrite(v, data);
            v += (ppuctrl & 4) ? 32 : 1;
            v %= 16384;
            ppudata = data;
        }
    }
}

uint8_t PPU::ppuread(uint16_t address) {
    switch (address) {
        case 0x0000 ... 0x1FFF:
            return rom->ppuread(address);
            break;
        case 0x2000 ... 0x2FFF:
            //Hardcoded horizontal mirroring
            if (address >= 0x2400 && address < 0x2800) {
               address -= 0x400;
            }
           
            if (address >= 0x2800 && address < 0x2c00) {
               address -= 0x400;
            }
           
            if (address >= 0x2c00 && address < 0x3000) {
               address -= 0x800;
            }
           
            return vram[address - 0x2000];
            break;
        case 0x3000 ... 0x3EFF:
            return ppuread(address - 0x1000);
            break;
        default:
            return 0;
            break;
    }
}

void PPU::ppuwrite(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x0000 ... 0x1FFF: rom->ppuwrite(address, data); break;
        case 0x2000 ... 0x2FFF:
            //Hardcoded horizontal mirroring
            if (address >= 0x2400 && address < 0x2800) {
                address -= 0x400;
            }
            
            if (address >= 0x2800 && address < 0x2c00) {
                address -= 0x400;
            }
            
            if (address >= 0x2c00 && address < 0x3000) {
                address -= 0x800;
            }
            
            vram[address - 0x2000] = data;
            break;
        case 0x3000 ... 0x3EFF:
            ppuwrite(address - 0x1000, data);
            break;
        default:
            break;
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
