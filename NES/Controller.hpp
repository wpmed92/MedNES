#ifndef Controller_hpp
#define Controller_hpp

#include <stdio.h>
#include <string>
#include "INESBus.hpp"

#include <SDL2/SDL.h>
class Controller : INESBus {
    uint8_t JOY1 = 0;
    uint8_t JOY2 = 0;
    uint8_t btnStateLocked = 0;
    uint8_t btnState = 0;
    bool strobe;
    
public:
    //Bus
    uint8_t* read(uint16_t address);
    void write(uint16_t address, uint8_t data);
    
    //Input
    void setButtonPressed(SDL_Keycode, bool);
    
};
#endif /* Controller_hpp */
