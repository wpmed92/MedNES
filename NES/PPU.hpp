#pragma once

#include <stdio.h>
#include <stdint.h>
#include "Common/Typedefs.hpp"
#include "INESBus.hpp"
#include "ROM.hpp"

struct Sprite {
    u8 y;
    u8 tileNum;
    u8 attr;
    u8 x;
    u8 id;
};

struct SpriteRenderEntity {
    u8 lo;
    u8 hi;
    u8 attr;
    u8 counter;
    u8 id;
    bool flipHorizontally;
    bool flipVertically;
    bool isActive;
    int shifted = 0;
    
    void shift() {
        if (shifted == 7) {
            isActive = false;
            shifted = 0;
            return;
        }
        
        if (flipHorizontally) {
            lo >>= 1;
            hi >>= 1;
        } else {
            lo <<= 1;
            hi <<= 1;
        }
        
        shifted++;
    }
};

class PPU : public INESBus {
private:
    //Registers
    u8 ppuctrl = 0; //$2000
    u8 ppumask = 0; //$2001
    u8 ppustatus = 0x80; //$2002
    u8 ppustatus_cpy = 0;
    u8 oamaddr = 0; //$2003
    u8 oamdata = 0; //$2004
    u8 ppuscroll = 0; //$2005
    u8 ppu_read_buffer = 0;
    u8 ppu_read_buffer_cpy = 0;
    
    u8 palette[192] =
    {
84,84,84,0,30,116,8,16,144,48,0,136,68,0,100,92,0,48,84,4,0,60,24,0,32,42,0,8,58,0,0,64,0,0,60,0,0,50,60,0,0,0,0,0,0,0,0,0,152,150,152,8,76,196,48,50,236,92,30,228,136,20,176,160,20,100,152,34,32,120,60,0,84,90,0,40,114,0,8,124,0,0,118,40,0,102,120,0,0,0,0,0,0,0,0,0,236,238,236,76,154,236,120,124,236,176,98,236,228,84,236,236,88,180,236,106,100,212,136,32,160,170,0,116,196,0,76,208,32,56,204,108,56,180,204,60,60,60,0,0,0,0,0,0,236,238,236,168,204,236,188,188,236,212,178,236,236,174,236,236,174,212,236,180,176,228,196,144,204,210,120,180,222,120,168,226,144,152,226,180,160,214,228,160,162,160,0,0,0,0,0,0
    };
    
    //BG
    u8 bg_palette[16] = { 0 };
    u8 vram[2048] = { 0 };
    u16 v = 0, t = 0, v1 = 0;
    u8 x = 0;
    int w = 0;
    u8 ntbyte, attrbyte, patternlow, patternhigh;
    //shifters
    u16 bgShiftRegLo;
    u16 bgShiftRegHi;
    u16 attrShiftReg1;
    u16 attrShiftReg2;
    
    //Sprites
    u8 sprite_palette[16] = { 0 };
    u16 spritePatternLowAddr, spritePatternHighAddr;
    int primaryOAMCursor = 0;
    int secondaryOAMCursor = 0;
    Sprite primaryOAM[64];
    Sprite secondaryOAM[8];
    Sprite tmpOAM;
    bool inRange = false;
    int inRangeCycles = 8;
    int spriteHeight = 8;
    int spriteDelayCounter = 4;
    //render entities
    std::vector<SpriteRenderEntity> spriteRenderEntities;

    SpriteRenderEntity out;
    ROM* rom;
    
    int scanLine = 0;
    int pixelIndex = 0;
    bool odd = false;
    bool nmiOccured = false;
    
    //methods
    inline void copyHorizontalBits();
    inline void copyVerticalBits();
    inline bool isRenderingDisabled();
    inline void emitPixel();
    inline void loadRegisters();
    inline void fetchTiles();
    inline void xIncrement();
    inline void yIncrement();
    inline void reloadShiftersAndShift();
    inline void decrementSpriteCounters();
    u16 getSpritePatternAddress(const Sprite&, bool);
    bool inNMISupressInterval();
    bool inVBlank();
    void evalSprites();
    bool inYRange(const Sprite&);
    bool isUninit(const Sprite&);
    
public:
    
    int dot = 0;
    PPU(ROM* rom) : rom(rom) { buffer = new uint32_t[256*240]; };
    
    //cpu address space
    u8* read(u16 address);
    void write(u16 address, u8 data);
    
    //ppu address space
    u8 ppuread(u16 address);
    void ppuwrite(u16 address, u8 data);
    
    void tick();
    void copyOAM(u8, int);
    bool genNMI();
    void drawFrame();
    bool generateFrame;
    void printState();
    uint32_t* buffer;
};
