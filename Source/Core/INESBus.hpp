#pragma once

#include <stdio.h>
#include <stdint.h>
#include "Common/Typedefs.hpp"

class INESBus {
public:
    virtual ~INESBus() {}
    virtual u8* read(u16 address) = 0;
    virtual void write(u16 address, u8 data) = 0;
};