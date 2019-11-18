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
        JOY1 = 0x40 | (btnState & 1);
        
        if (strobe) {
            return &JOY1;
        }
        
        JOY1 = 0x80 | (btnStateLocked & 1);
        btnStateLocked >>= 1;
        return &JOY1;
    } else {
        //TODO: Implement JOY2
        return &JOY2;
    }
}

void Controller::write(uint16_t address, uint8_t data) {
    if (address == 0x4016) {
        if (strobe && !(data & 0x1)) {
            btnStateLocked = btnState;
        }
        
        strobe = data & 0x1;
    } else {
        //TODO: Implement JOY2
    }
}

void Controller::setButtonPressed(SDL_Keycode key, bool pressed) {
    if (key == SDLK_a) {
        btnState = (pressed) ? (btnState | (1 << 0)) : (btnState & ~(1 << 0));
    }
    
    if (key == SDLK_b) {
        btnState  = (pressed) ? (btnState | (1 << 1)) : (btnState & ~(1 << 1));
    }
    
    if (key == SDLK_SPACE) {
        btnState  = (pressed) ? (btnState | (1 << 2)) : (btnState & ~(1 << 2));
    }
    
    if (key == SDLK_RETURN) {
        btnState = (pressed) ? (btnState | (1 << 3)) : (btnState & ~(1 << 3));
    }
    
    if (key == SDLK_UP) {
        btnState = (pressed) ? (btnState | (1 << 4)) : (btnState & ~(1 << 4));
    }
    
    if (key == SDLK_DOWN) {
        btnState = (pressed) ? (btnState | (1 << 5)) : (btnState & ~(1 << 5));
    }
    
    if (key == SDLK_LEFT) {
        btnState = (pressed) ? (btnState | (1 << 6)) : (btnState & ~(1 << 6));
    }
    
    if (key == SDLK_RIGHT) {
        btnState = (pressed) ? (btnState | (1 << 7)) : (btnState & ~(1 << 7));
    }
}
