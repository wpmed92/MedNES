#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <bitset>
#include "INESBus.hpp"
#include "Mapper/NROM.hpp"
#include "Mapper/UnROM.hpp"

struct INESHeader {
    //Header 16 byte
    char nes[4];
    uint8_t prgIn16kb;
    uint8_t chrIn8kb;
    uint8_t flags6;
    uint8_t flags7;
    uint8_t prgRamIn8kb;
    uint8_t flags9;
    uint8_t flags10;
    char zeros[5];
};

class ROM : public INESBus {
    
private:
    INESHeader header;
    std::vector<uint8_t> trainer;
    std::vector<uint8_t> prgCode;
    std::vector<uint8_t> chrData;
    uint8_t chrRAM[8192];
    int mirroring;
    uint8_t mapperNum;
    
public:
    std::vector<uint8_t> getChrData() { return chrData; };
    std::vector<uint8_t> getPrgCode() { return prgCode; };
    void open(std::string);
    void printHeader();
    void loadTestProgramcode(std::vector<uint8_t>);
    int getMirroring();
    Mapper* getMapper();
    
    //cpu address space
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);

    //ppu address space
    uint8_t ppuread(uint16_t address);
    void ppuwrite(uint16_t address, uint8_t data);
};
