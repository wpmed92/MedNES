#pragma once

#include <stdint.h>
#include <stdio.h>

#include "Common/Typedefs.hpp"

namespace MedNES {

class INESBus {
   public:
    virtual ~INESBus() {}
    virtual u8 read(u16 address) = 0;
    virtual void write(u16 address, u8 data) = 0;
};

};  //namespace MedNES
