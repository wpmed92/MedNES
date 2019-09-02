//
//  RAM.hpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 12..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#ifndef RAM_hpp
#define RAM_hpp

#include <stdio.h>
#include <stdint.h>
#include "INESBus.hpp"

class RAM : public INESBus {
    
//256 byte pages, 8 pages on internal NES RAM
private:
    uint8_t ram[2048];
    
public:
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
};

#endif /* RAM_hpp */
