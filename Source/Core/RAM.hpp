#pragma once

#include <stdint.h>
#include <stdio.h>

#include "Common/Typedefs.hpp"
#include "INESBus.hpp"

namespace MedNES {

class RAM : public INESBus {
   public:
    u8 read(u16 address);
    void write(u16 address, u8 data);

    //256 byte pages, 8 pages on internal NES RAM
   private:
    u8 ram[2048] = {0};
};

};  //namespace MedNES
