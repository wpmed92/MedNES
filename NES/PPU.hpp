#ifndef PPU_hpp
#define PPU_hpp

#include <stdio.h>
#include <stdint.h>
#include "INESBus.hpp"
#include "ROM.hpp"

struct Sprite {
    uint8_t y;
    uint8_t tileNum;
    uint8_t attr;
    uint8_t x;
};

struct SpriteRenderEntity {
    uint8_t lo;
    uint8_t hi;
    uint8_t attr;
    uint8_t counter;
    bool isActive;
    int shifted;
    
    void shift() {
        if (shifted == 8) {
            isActive = false;
            return;
        }
        
        lo <<= 1;
        hi <<= 1;
        shifted++;
    }
};

class PPU : public INESBus {
private:
    //Registers
    uint8_t ppuctrl = 0; //$2000
    uint8_t ppumask = 0; //$2001
    uint8_t ppustatus = 0x80; //$2002
    uint8_t ppustatus_cpy = 0;
    uint8_t oamaddr = 0; //$2003
    uint8_t oamdata = 0; //$2004
    uint8_t ppuscroll = 0; //$2005
    uint8_t ppu_read_buffer = 0;
    uint8_t ppu_read_buffer_cpy = 0;
    
    uint8_t palette[192] =
    {
84,84,84,0,30,116,8,16,144,48,0,136,68,0,100,92,0,48,84,4,0,60,24,0,32,42,0,8,58,0,0,64,0,0,60,0,0,50,60,0,0,0,0,0,0,0,0,0,152,150,152,8,76,196,48,50,236,92,30,228,136,20,176,160,20,100,152,34,32,120,60,0,84,90,0,40,114,0,8,124,0,0,118,40,0,102,120,0,0,0,0,0,0,0,0,0,236,238,236,76,154,236,120,124,236,176,98,236,228,84,236,236,88,180,236,106,100,212,136,32,160,170,0,116,196,0,76,208,32,56,204,108,56,180,204,60,60,60,0,0,0,0,0,0,236,238,236,168,204,236,188,188,236,212,178,236,236,174,236,236,174,212,236,180,176,228,196,144,204,210,120,180,222,120,168,226,144,152,226,180,160,214,228,160,162,160,0,0,0,0,0,0
    };
    
    //BG
    uint8_t bg_palette[16] = { 0 };
    uint8_t vram[2048] = { 0 };
    uint16_t v = 0, t = 0, v1 = 0;
    uint8_t x = 0;
    int w = 0;
    uint8_t ntbyte, attrbyte, patternlow, patternhigh;
    //shifters
    uint16_t bgShiftRegLo;
    uint16_t bgShiftRegHi;
    uint16_t attrShiftReg1;
    uint16_t attrShiftReg2;
    
    //Sprites
    uint8_t sprite_palette[16] = { 0 };
    uint16_t spritePatternLowAddr, spritePatternHighAddr;
    int primaryOAMCursor = 0;
    int secondaryOAMCursor = 0;
    Sprite primaryOAM[64];
    Sprite secondaryOAM[8];
    Sprite tmpOAM;
    bool inRange = false;
    int inRangeCycles = 8;
    int spriteHeight = 8;
    //render entities
    SpriteRenderEntity spriteRenderEntities[8];
    
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
    uint16_t getSpritePatternAddress(const Sprite&);
    bool inNMISupressInterval();
    bool inVBlank();
    void evalSprites();
    uint8_t mux(uint8_t, uint8_t);
    bool inYRange(const Sprite&);
    
public:
    
    int dot = 0;
    PPU(ROM* rom) : rom(rom) { buffer = new uint32_t[256*240]; };
    //cpu address space
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
    
    //ppu address space
    uint8_t ppuread(uint16_t address);
    void ppuwrite(uint16_t address, uint8_t data);
    
    void tick();
    void copyOAM(uint8_t, int);
    bool genNMI();
    void drawFrame();
    bool generateFrame;
    void printState();
    uint32_t* buffer;
};



#endif /* PPU_hpp */
