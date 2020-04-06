#pragma once

#include <stdio.h>
#include <stdint.h>
#include "Common/Typedefs.hpp"
#include "INESBus.hpp"

class RAM : public INESBus {
    
//256 byte pages, 8 pages on internal NES RAM
private:
    u8 ram[2048] = {0};
    
public:
    u8 read(u16 address);
    void write(u16 address, u8 data);
};