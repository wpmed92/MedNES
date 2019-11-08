#include "PPU.hpp"
#include <iostream>

void PPU::tick() {
     if ((scanLine >= 0 && scanLine <= 239) || scanLine == 261) { //visible scanline, pre-render scanline
        if (scanLine == 261) {
            //clear vbl flag and sprite overflow
            if (dot == 1) {
                ppustatus &= ~0x80;
                ppustatus &= ~0x20;
            }
            
            //copy vertical bits
            if (dot > 280 && dot < 304) {
                copyVerticalBits();
            }
        }
    
        //main hook: fetch tiles, emit pixel, shift
        if ((dot >= 1 && dot <= 257) || (dot >= 321 && dot <= 337)) {
            if (dot == 257) {
                copyHorizontalBits();
            }
            
            //reload shift registers and shift
            if ((dot >= 2 && dot <= 257) || (dot >= 322 && dot <= 337)) {
                reloadShiftersAndShift();
            }
            
            //if on visible scanlines and dots
            //eval sprites
            //emit pixels
            if ((scanLine >= 0 && scanLine <= 239)) {
                evalSprites();
                
                if (dot >= 1 && dot <= 256) {
                    emitPixel();
                }
            }
            
            //fetch nt, at, pattern low - high
            fetchTiles();
        }
    } else if (scanLine >= 240 && scanLine <= 260) { //post-render, vblank
        if (scanLine == 240 && dot == 0) {
            generateFrame = true;
            pixelIndex = 0;
        }
        
        if (scanLine == 241 && dot == 1) {
            //set vbl flag
            ppustatus |= 0x80;

            //flag for nmi
            if (ppuctrl & 0x80) {
                nmiOccured = true;
            }
        }
    }

    //skip dot on odd frame
    if (odd && !isRenderingDisabled() && dot == 339 && scanLine == 261) {
        dot++;
    }
    
    if (dot == 340) {
        scanLine = (scanLine+1) % 262;
        if (scanLine == 0) {
            odd = !odd;
        }
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

inline void PPU::reloadShiftersAndShift() {
    if (isRenderingDisabled()) {
        return;
    }
    
    if (dot % 8 == 1) {
        bgShiftRegLo |= patternlow;
        bgShiftRegHi |= patternhigh;
    }
    
    bgShiftRegLo <<= 1;
    bgShiftRegHi <<= 1;
    attrShiftReg1 <<= 1;
    attrShiftReg2 <<= 1;
}

inline bool PPU::isRenderingDisabled() {
    return !((ppumask & 8) || (ppumask & 16));
}

inline void PPU::fetchTiles() {
    if (isRenderingDisabled()) {
        return;
    }
    
    if (dot % 8 == 1) {
        ntbyte = ppuread(0x2000 | (v & 0x0FFF));
    }
    
    if (dot % 8 == 3) {
        attrbyte = ppuread(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
    }
    
    if (dot % 8 == 5) {
        uint16_t patterAddr =
        (((uint16_t) ppuctrl & 0x10) << 8) +
        ((uint16_t) ntbyte << 4) +
        ((v & 0x7000) >> 12);
        patternlow = ppuread(patterAddr);
    }
    
    if (dot % 8 == 7) {
        uint16_t patterAddr =
        (((uint16_t) ppuctrl & 0x10) << 8) +
         ((uint16_t) ntbyte << 4) +
         ((v & 0x7000) >> 12) + 8;
        patternhigh = ppuread(patterAddr);
    }
    
    if (dot % 8 == 0) {
        uint8_t quadrant_num = (((v & 2) >> 1) | ((v & 64) >> 5)) * 2;
        uint8_t attr_bits1 = (attrbyte >> quadrant_num) & 1;
        uint8_t attr_bits2 = (attrbyte >> (quadrant_num + 1)) & 1;
        attrShiftReg1 |= attr_bits1 ? 255 : 0;
        attrShiftReg2 |= attr_bits2 ? 255 : 0;
        
        if (dot == 256) {
            yIncrement();
            xIncrement();
        } else {
            xIncrement();
        }
    }
}

inline void PPU::emitPixel() {
    if (isRenderingDisabled()) {
        return;
    }
    
    uint16_t fineSelect = 0x8000 >> x;
    uint16_t pixel1 = (bgShiftRegLo & fineSelect) << x;
    uint16_t pixel2 = (bgShiftRegHi & fineSelect) << x;
    uint16_t pixel3 = (attrShiftReg1 & fineSelect) << x;
    uint16_t pixel4 = (attrShiftReg2 & fineSelect) << x;
    uint8_t paletteIndex = 0 | (pixel4 >> 12) | (pixel3 >> 13) | (pixel2 >> 14) | (pixel1 >> 15);
    
    //When bg rendering is off
    if ((ppumask & 8) == 0) {
        paletteIndex = 0;
    }
    
    uint8_t p = ppuread(0x3F00 | paletteIndex) * 3;
    uint8_t r = palette[p];
    uint8_t g = palette[p + 1];
    uint8_t b = palette[p + 2];
    buffer[pixelIndex++] = 255 << 24 | r << 16 | g << 8 | b;
}

inline void PPU::copyHorizontalBits() {
    if (isRenderingDisabled()) {
        return;
    }
    
    v = (v & ~0x41F) | (t & 0x41F);
}

inline void PPU::copyVerticalBits() {
    if (isRenderingDisabled()) {
        return;
    }
    
    v = (v & ~0x7BE0) | (t & 0x7BE0);
}

bool PPU::genNMI() {
    if (nmiOccured == true) {
        std::cout << "NMI occured: scanline=" << unsigned(scanLine) << ", dot=" << unsigned(dot) << std::endl;
        nmiOccured = false;
        return true;
    } else {
        return false;
    }
    
}

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
    } else if (address == 7) {
        ppu_read_buffer = ppu_read_buffer_cpy;
        ppu_read_buffer_cpy = ppuread(v);
        v += ((ppuctrl & 4) ? 32 : 1);
        v%=16384;
        return &ppu_read_buffer;
    }
    
    return nullptr;
}

void PPU::write(uint16_t address, uint8_t data) {
    address %= 8;
    
    if (address == 0) {
        t = (t & 0xF3FF) | (((uint16_t) data & 0x03) << 10);
        ppuctrl = data;
    } else if (address == 1) {
        ppumask = data;
    } else if (address == 2) {
        data &= ~128;
        ppustatus &= 128;
        ppustatus |= data;
    } else if (address == 3) {
        oamaddr = data;
    } else if (address == 4) {
        oamdata = data;
    } else if (address == 5) {
        if (w == 0) {
            t &= 0x7FE0;
            t |= ((uint16_t) data) >> 3;
            x = data & 7;
            w = 1;
        } else {
            t &= ~0x73E0;
            t |= ((uint16_t) data & 0x07) << 12;
            t |= ((uint16_t) data & 0xF8) << 2;
            w = 0;
        }
        
        ppuscroll = data;
    } else if (address == 6) {
        if (w == 0) {
            t &= 255;
            t |= ((uint16_t)data & 0x3F) << 8;
            w = 1;
        } else {
            t &= 0xFF00;
            t |= data;
            v = t;
            
            if (!isRenderingDisabled() && !inVBlank()) {
                std:: cout << "V set: " << unsigned(v);
                printState();
            }
            
            w = 0;
        }
    } else if (address == 7) {
        ppuwrite(v, data);
        v += ((ppuctrl & 4) ? 32 : 1);
        v %= 16384;
    }
}

uint8_t PPU::ppuread(uint16_t address) {
    switch (address) {
        case 0x0000 ... 0x1FFF:
            return rom->ppuread(address);
            break;
        case 0x2000 ... 0x2FFF:
            //Horizontal
            if (rom->getMirroring() == 0) {
                if (address >= 0x2400 && address < 0x2800) {
                    address -= 0x400;
                }

                if (address >= 0x2800 && address < 0x2c00) {
                    address -= 0x400;
                }

                if (address >= 0x2c00 && address < 0x3000) {
                    address -= 0x800;
                }
            //Vertical
            } else {
                if (address >= 0x2800 && address < 0x3000) {
                    address -= 0x800;
                }
            }
        
            return vram[address - 0x2000];
            break;
        case 0x3F00 ... 0x3F0F:
            if (address == 0x3F04 || address == 0x3F08 || address == 0x3F0C) {
                address = 0x3F00;
            }
            
            return bg_palette[address - 0x3F00];
            break;
        case 0x3F10 ... 0x3F1F:
            if (address == 0x3F10 || address == 0x3F14 || address == 0x3F18 || address == 0x3F1C) {
                return ppuread(address & 0x3F0F);
            }
            
            return 1;
            break;
        case 0x3000 ... 0x3EFF:
            return ppuread(address - 0x1000);
            break;
        default:
            return 1;
            break;
    }
}

void PPU::ppuwrite(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x0000 ... 0x1FFF:
            rom->ppuwrite(address, data);
            break;
        case 0x2000 ... 0x2FFF:
            //Horizontal
            if (rom->getMirroring() == 0) {
                if (address >= 0x2400 && address < 0x2800) {
                    address -= 0x400;
                }

                if (address >= 0x2800 && address < 0x2c00) {
                    address -= 0x400;
                }

                if (address >= 0x2c00 && address < 0x3000) {
                    address -= 0x800;
                }
            //Vertical
            } else {
                if (address >= 0x2800 && address < 0x3000) {
                    address -= 0x800;
                }
            }

            vram[address - 0x2000] = data;
            break;
        case 0x3F00 ... 0x3F0F:
            bg_palette[address - 0x3F00] = data;
            break;
        case 0x3F10 ... 0x3F1F:
            if (address == 0x3F10) {
                bg_palette[0] = data;
            } else if (address == 0x3F14) {
                bg_palette[4] = data;
            } else if (address == 0x3F18) {
                bg_palette[8] = data;
            } else if (address == 0x3F1C) {
                bg_palette[12] = data;
            }
            
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

void PPU::evalSprites() {
    if (isRenderingDisabled()) {
        return;
    }
    
    //clear secondary OAM
    if (dot >= 1 && dot <= 64) {
        if (dot == 1) {
            secondaryOAMCursor = 0;
        }
        
        secondaryOAM[secondaryOAMCursor].spriteAttr = 0xFF;
        secondaryOAM[secondaryOAMCursor].spriteTileNum = 0xFF;
        secondaryOAM[secondaryOAMCursor].spriteX = 0xFF;
        secondaryOAM[secondaryOAMCursor].spriteY = 0xFF;
        
        if (dot % 8 == 0) {
            secondaryOAMCursor++;
        }
    }
    
    //sprite eval
    if (dot >= 65 && dot <= 256) {
        if (dot == 65) {
            secondaryOAMCursor = 0;
            primaryOAMCursor = 0;
        }
        
        //odd cycle read
        if ((dot % 2) == 1) {
            tmpOAM = primaryOAM[primaryOAMCursor];
            
            if (tmpOAM.spriteY == scanLine) {
                inRangeCycles--;
                inRange = true;
            }
        //even cycle write
        } else {
            //tmpOAM is in range, write it to secondaryOAM
            if (inRange) {
                inRangeCycles--;

                //copying tmpOAM in range is 8 cycles, 2 cycles otherwise
                if (inRangeCycles == 0) {
                    primaryOAMCursor++;
                    secondaryOAMCursor++;
                    inRangeCycles = 8;
                    inRange = false;
                } else {
                    if (secondaryOAMCursor == 8) {
                        //Sprite overflow
                        ppustatus |= 0x20;
                    } else {
                        secondaryOAM[secondaryOAMCursor] = tmpOAM;
                    }
                }
            } else {
                primaryOAMCursor = (primaryOAMCursor+1) % 64;
            }
        }
    }
    
    //Sprite fetches
    if (dot >= 257 && dot <= 320) {
       
    }
}

void PPU::printState() {
    std::cout << "scanline=" << unsigned(scanLine) << ", " << unsigned(dot) << std::endl << std::endl;
}

bool PPU::inVBlank() {
    return (scanLine >= 241 && scanLine <= 260);
}
