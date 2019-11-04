#ifndef Controller_hpp
#define Controller_hpp

#include <stdio.h>
#include <string>
#include "INESBus.hpp"

class Controller : INESBus {
    uint8_t JOY1 = 0;
    uint8_t JOY2 = 0;
    uint8_t shifter = 0;
    
public:
    bool shouldPoll;
    //Bus
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
    
    //Input
    void setButtonState(std::string);
    void resetButtonState();
    
};
#endif /* Controller_hpp */
