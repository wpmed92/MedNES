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

struct addr {
    uint8_t hi;
    uint8_t lo;
};

class PPU : public INESBus {
private:
    
    //Registers
    uint8_t ppuctrl = 0; //$2000
    uint8_t ppumask = 0; //$2001
    uint8_t ppustatus = 0; //$2002
    uint8_t ppustatus_cpy = 0;
    uint8_t oamaddr = 0; //$2003
    uint8_t oamdata = 0; //$2004
    uint8_t ppuscroll = 0; //$2005
    uint8_t ppu_read_buffer = 0;
    uint8_t ppu_read_buffer_cpy = 0;
    
    uint8_t palette[192] =
    {
  84,84,84, 0,30,116, 8,16,144, 48,0,136, 68,0,100, 92,0,48, 84,4,0, 60,24,0,32,42,0,8,58,0,0,64,0,0,60,0,0,50,60,0,0,0,152,150,152,8,76,196,48,50,236,92,30,228,136,20,176,160,20,100,152,34,32,120,60,0,84,90,0,40,114,0,8,124,0,0,118,40,0,102,120,0,0,0,236,238,236,76,154,236,120,124,236,176,98,236,228,84,236,236,88,180,236,106,100,212,136,32,160,170,0,116,196,0,76,208,32,56,204,108,56,180,204,60,60,60,236,238,236,168,204,236,188,188,236,212,178,236,236,174,236,236,174,212,236,180,176,228,196,144,204,210,120,180,222,120,168
        
    };
    
    //Nametable vram
    uint8_t vram[2048] = { 0 };
    
    //current vram address, temporary vram address
    uint16_t v = 0, t = 0, tscroll = 0;
    
    //fine x scroll
    uint8_t x = 0;
    
    //first/second write toggle
    int w = 0;
    addr testV;
    uint8_t ntbyte, attrbyte, patternlow, patternhigh;
    
    uint16_t shiftReg1;
    uint16_t shiftReg2;
    
    //Object Attribute Memory
    uint8_t oamdma; //$4014
    
    OAM primaryOAM[64];
    
    OAM secondaryOAM[8];
    
    ROM* rom;
    
    //Scanline
    int scanLine = 0;
    int pixelIndex = 0;
    bool odd = false;
    bool nmiOccured = false;
    inline bool isRenderingDisabled();
    inline void emitPixel();
    inline void loadRegisters();
    inline void fetchTiles();
    inline void xIncrement();
    inline void yIncrement();
    
public:
    
    int dot = 0;
    PPU(ROM* rom) : rom(rom) { };
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
    void printNametable();
    uint8_t frame[256*240] = { 0 };
};



#endif /* PPU_hpp */
