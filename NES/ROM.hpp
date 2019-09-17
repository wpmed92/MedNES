//
//  ROM.hpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 21..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#ifndef ROM_hpp
#define ROM_hpp

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "INESBus.hpp"

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
    
public:
    std::vector<uint8_t> getChrData() { return chrData; };
    void open(std::string);
    void printHeader();
    void loadTestProgramcode(std::vector<uint8_t>);
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
    
};

#endif /* ROM_hpp */
