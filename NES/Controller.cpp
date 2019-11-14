//
//  Controller.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2019. 09. 20..
//  Copyright © 2019. Ahmed Harmouche. All rights reserved.
//

#include "Controller.hpp"

uint8_t* Controller::read(uint16_t address) {
    if (address == 0x4016) {
        JOY1 = 0x40 | (shifter & 1);
        
        if (strobe) {
            return &JOY1;
        }
        
        if (shift) {
            shifter >>= 1;
        }
        return &JOY1;
    } else {
        //TODO: Implement JOY2
        return &JOY2;
    }
}

void Controller::write(uint16_t address, uint8_t data) {
    if (address == 0x4016) {
        shift = strobe && !data;
        strobe = shouldPoll = data;
        
        if (shouldPoll) {
            shifter = shifter1;
        }
    
    } else {
        //TODO: Implement JOY2
    }
}

void Controller::setButtonPressed(SDL_Keycode key, bool pressed) {
    if (!shouldPoll)
        return;
    
    if (key == SDLK_a) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 0)) : (shifter & ~(1 << 0));
    }
    
    if (key == SDLK_b) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 1)) : (shifter & ~(1 << 1));
    }
    
    if (key == SDLK_SPACE) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 2)) : (shifter & ~(1 << 2));
    }
    
    if (key == SDLK_RETURN) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 3)) : (shifter & ~(1 << 3));
    }
    
    if (key == SDLK_UP) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 4)) : (shifter & ~(1 << 4));
    }
    
    if (key == SDLK_DOWN) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 5)) : (shifter & ~(1 << 5));
    }
    
    if (key == SDLK_LEFT) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 6)) : (shifter & ~(1 << 6));
    }
    
    if (key == SDLK_RIGHT) {
        shifter1 = shifter = (pressed) ? (shifter | (1 << 7)) : (shifter & ~(1 << 7));
    }
}
