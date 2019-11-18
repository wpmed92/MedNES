#include "PPU.hpp"
#include <iostream>

void PPU::tick() {
     if ((scanLine >= 0 && scanLine <= 239) || scanLine == 261) { //visible scanline, pre-render scanline
        if (scanLine == 261) {
            //clear vbl flag and sprite overflow
            if (dot == 2) {
                pixelIndex = 0;
                ppustatus &= ~0x80;
                ppustatus &= ~0x20;
                ppustatus &= ~64;
            }
            
            //copy vertical bits
            if (dot >= 280 && dot <= 304) {
                copyVerticalBits();
            }
        }

         //skip dot on odd frame
         /*if (odd && !isRenderingDisabled() && dot == 339 && scanLine == 261) {
             scanLine = 0;
             dot = 0;
             tick();
         }*/
         
        if (scanLine >= 0 && scanLine <= 239) {
            evalSprites();
        }

        if (dot == 257) {
            copyHorizontalBits();
        }
         
        //main hook: fetch tiles, emit pixel, shift
        if ((dot >= 1 && dot <= 257) || (dot >= 321 && dot <= 337)) {
            //reload shift registers and shift
            if ((dot >= 2 && dot <= 257) || (dot >= 322 && dot <= 337)) {
                reloadShiftersAndShift();
            }
            
            //if on visible scanlines and dots
            //eval sprites
            //emit pixels
            if (scanLine >= 0 && scanLine <= 239) {
                if (dot >= 1 && dot <= 256) {
                    if (scanLine > 0) {
                        decrementSpriteCounters();
                    }
                    
                    emitPixel();
                }
            }
            
            //fetch nt, at, pattern low - high
            fetchTiles();
        }
    } else if (scanLine >= 240 && scanLine <= 260) { //post-render, vblank
        if (scanLine == 240 && dot == 0) {
            generateFrame = true;
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
    
    int cycle = dot % 8 ;
    
    if (cycle == 1) {
        ntbyte = ppuread(0x2000 | (v & 0x0FFF));
    } else if (cycle == 3) {
        attrbyte = ppuread(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
    } else if (cycle == 5) {
        uint16_t patterAddr =
        (((uint16_t) ppuctrl & 0x10) << 8) +
        ((uint16_t) ntbyte << 4) +
        ((v & 0x7000) >> 12);
        patternlow = ppuread(patterAddr);
    } else if (cycle == 7) {
        uint16_t patterAddr =
        (((uint16_t) ppuctrl & 0x10) << 8) +
         ((uint16_t) ntbyte << 4) +
         ((v & 0x7000) >> 12) + 8;
        patternhigh = ppuread(patterAddr);
    } else if (cycle == 0) {
        uint8_t quadrant_num = (((v & 2) >> 1) | ((v & 64) >> 5)) * 2;
        uint8_t attr_bits1 = (attrbyte >> quadrant_num) & 1;
        uint8_t attr_bits2 = (attrbyte >> (quadrant_num + 1)) & 1;
        attrShiftReg1 |= attr_bits1 ? 255 : 0;
        attrShiftReg2 |= attr_bits2 ? 255 : 0;
        
        if (dot == 256) {
            yIncrement();
        }
        
        xIncrement();
        
    }
}

inline void PPU::emitPixel() {
    if (isRenderingDisabled()) {
        return;
    }
    
    //Bg
    uint16_t fineSelect = 0x8000 >> x;
    uint16_t pixel1 = (bgShiftRegLo & fineSelect) << x;
    uint16_t pixel2 = (bgShiftRegHi & fineSelect) << x;
    uint16_t pixel3 = (attrShiftReg1 & fineSelect) << x;
    uint16_t pixel4 = (attrShiftReg2 & fineSelect) << x;
    uint8_t bgBit12 = (pixel2 >> 14) | (pixel1 >> 15);
    
    //Sprites
    uint8_t spritePixel1 = 0;
    uint8_t spritePixel2 = 0;
    uint8_t spritePixel3 = 0;
    uint8_t spritePixel4 = 0;
    uint8_t spriteBit12 = 0;
    uint8_t paletteIndex = 0 | (pixel4 >> 12) | (pixel3 >> 13) | (pixel2 >> 14) | (pixel1 >> 15);
    uint8_t spritePaletteIndex = 0;
    bool showSprite = false;
    bool spriteFound = false;
    
    for (int i = 0; i < spriteRenderEntities.size(); i++) {
        if (spriteRenderEntities[i].isActive) {
            SpriteRenderEntity &sprite = spriteRenderEntities[i];
            
            if (spriteFound) {
                sprite.shift();
                continue;
            }
            
            spritePixel1 = sprite.flipHorizontally ? ((sprite.lo & 1) << 7) : sprite.lo & 128;
            spritePixel2 = sprite.flipHorizontally ? ((sprite.hi & 1) << 7) : sprite.hi & 128;
            spritePixel3 = sprite.attr & 1;
            spritePixel4 = sprite.attr & 2;
            spriteBit12 = (spritePixel2 >> 6) | (spritePixel1 >> 7);
            
            if (!(ppustatus & 64) && spriteBit12 && bgBit12 && sprite.id == 0 && (ppumask & 16) && (ppumask & 8)) {
                ppustatus |= 64;
            }
            
            if (spriteBit12) {
                showSprite = ((spriteBit12 && bgBit12 && !(sprite.attr & 32)) || (spriteBit12 && !bgBit12)) && (ppumask & 16);
                spritePaletteIndex = 0x10 | (spritePixel4 << 2) | (spritePixel3 << 2) | spriteBit12;
                spriteFound = true;
            }

            sprite.shift();
        }
    }
    
    //When bg rendering is off
    if ((ppumask & 8) == 0) {
        paletteIndex = 0;
    }
    
    if (showSprite) {
        uint8_t p = ppuread(0x3F00 | spritePaletteIndex) * 3;
        uint8_t r = palette[p];
        uint8_t g = palette[p + 1];
        uint8_t b = palette[p + 2];
        buffer[pixelIndex++] = 255 << 24 | r << 16 | g << 8 | b;
    } else {
        uint8_t p = ppuread(0x3F00 | paletteIndex) * 3;
        uint8_t r = palette[p];
        uint8_t g = palette[p + 1];
        uint8_t b = palette[p + 2];
        buffer[pixelIndex++] = 255 << 24 | r << 16 | g << 8 | b;
    }
}

uint8_t mux(uint8_t bg, uint8_t sprite) {
    return 0;
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
        
        if (v >= 0x3F00 && v <= 0x3FFF) {
            ppu_read_buffer_cpy = ppuread(v - 0x1000);
            ppu_read_buffer = ppuread(v);
        } else {
            ppu_read_buffer_cpy = ppuread(v);
        }
        
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
        spriteHeight = (data & 0x20) ? 16 : 8;
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
            } else {
                return sprite_palette[address - 0x3F10];
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
            if (address == 0x3F10 || address == 0x3F14 || address == 0x3F18 || address == 0x3F1C) {
                bg_palette[(address & 0x3F0F) - 0x3F00] = data;
            } else if (address >= 0x3F11 && address <= 0x3F1F) {
                sprite_palette[address - 0x3F10] = data;
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
        primaryOAM[oamSelect].y = oamEntry;
    } else if (property == 1) {
        primaryOAM[oamSelect].tileNum = oamEntry;
    } else if (property == 2) {
        primaryOAM[oamSelect].attr = oamEntry;
    } else {
        primaryOAM[oamSelect].x = oamEntry;
    }
}

inline void PPU::decrementSpriteCounters() {
    if (isRenderingDisabled()) {
        return;
    }
    
    for (int i = 0; i < spriteRenderEntities.size(); i++) {
        if (spriteRenderEntities.size() == 0)
            break;
        if (spriteRenderEntities[i].counter != 0) {
           spriteRenderEntities[i].counter--;
            
            if (spriteRenderEntities[i].counter == 0) {
                spriteRenderEntities[i].isActive = true;
            }
        }
    }
}

bool PPU::isUninit(const Sprite& sprite) {
    return ((sprite.attr == 0xFF) && (sprite.tileNum == 0xFF) && (sprite.x == 0xFF) && (sprite.y == 0xFF)) || ((sprite.x == 0) && (sprite.y == 0) && (sprite.attr == 0) && (sprite.tileNum == 0));
}

void PPU::evalSprites() {
    //clear secondary OAM
    if (dot >= 1 && dot <= 64) {
        if (dot == 1) {
            secondaryOAMCursor = 0;
        }
        
        secondaryOAM[secondaryOAMCursor].attr = 0xFF;
        secondaryOAM[secondaryOAMCursor].tileNum = 0xFF;
        secondaryOAM[secondaryOAMCursor].x = 0xFF;
        secondaryOAM[secondaryOAMCursor].y = 0xFF;
        
        if (dot % 8 == 0) {
            secondaryOAMCursor++;
        }
    }
    
    //sprite eval
    if (dot >= 65 && dot <= 256) {
        //Init
        if (dot == 65) {
            secondaryOAMCursor = 0;
            primaryOAMCursor = 0;
        }
        
        if (secondaryOAMCursor == 8) {
            //ppustatus |= 0x20;
            return;
        }
        
        if (primaryOAMCursor == 64) {
            return;
        }
        
        //odd cycle read
        if ((dot % 2) == 1) {
            tmpOAM = primaryOAM[primaryOAMCursor];
            
            if (inYRange(tmpOAM)) {
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
                    tmpOAM.id = primaryOAMCursor;
                    secondaryOAM[secondaryOAMCursor] = tmpOAM;
                }
            } else {
                primaryOAMCursor++;
            }
        }
    }
    
    //Sprite fetches
    if (dot >= 257 && dot <= 320) {
        if (dot == 257) {
            secondaryOAMCursor = 0;
            spriteRenderEntities.clear();
        }
        
        Sprite sprite = secondaryOAM[secondaryOAMCursor];
        
        int cycle = (dot-1) % 8;
        
        switch (cycle) {
            case 0 ... 1:
                if (!isUninit(sprite))
                    out = SpriteRenderEntity();
                break;
                
            case 2:
                if (!isUninit(sprite)) {
                    out.attr = sprite.attr;
                    out.flipHorizontally = sprite.attr & 64;
                    out.flipVertically = sprite.attr & 128;
                    out.id = sprite.id;
                }
                break;
                
            case 3:
                if (!isUninit(sprite))
                    out.counter = sprite.x;
                break;
                
            case 4:
                if (!isUninit(sprite)) {
                    spritePatternLowAddr = getSpritePatternAddress(sprite, out.flipVertically);
                    out.lo = ppuread(spritePatternLowAddr);
                }
                break;
                
            case 5:
                break;
                
            case 6:
                if (!isUninit(sprite)) {
                    spritePatternHighAddr = spritePatternLowAddr + 8;
                    out.hi = ppuread(spritePatternHighAddr);
                }
                break;
                
            case 7:
                if (!isUninit(sprite))
                    spriteRenderEntities.push_back(out);
                
                secondaryOAMCursor++;
                break;
                
            default:
                break;
        }
    }
}

uint16_t PPU::getSpritePatternAddress(const Sprite &sprite, bool flipVertically) {
    uint16_t addr = 0;
    
    if (spriteHeight == 8) {
        int fineOffset = scanLine - sprite.y;
        
        if (flipVertically)
            fineOffset = spriteHeight-1 - fineOffset;
        
        addr = (((uint16_t) ppuctrl & 8) << 9) |
        ((uint16_t) sprite.tileNum << 4) |
        fineOffset;
    } else {
        
        int fineOffset = scanLine - sprite.y;
        
        if (flipVertically)
            fineOffset = spriteHeight-1 - fineOffset;
        addr = (((uint16_t) sprite.tileNum & 1) << 12) |
        ((uint16_t) ((sprite.tileNum & ~1) << 4)) |
        fineOffset;
    }
    
    return addr;
}

bool PPU::inYRange(const Sprite& oam) {
    return !isUninit(oam) && ((scanLine >= oam.y) && (scanLine < (oam.y + spriteHeight)));
}

void PPU::printState() {
    std::cout << "scanline=" << unsigned(scanLine) << ", " << unsigned(dot) << std::endl << std::endl;
}

bool PPU::inVBlank() {
    return (scanLine >= 241 && scanLine <= 260);
}
