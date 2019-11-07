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
        JOY1 = shifter & 1;
        shifter >>= 1;
        
        return &JOY1;
    } else {
        //TODO: Implement JOY2
        return &JOY2;
    }
}

void Controller::write(uint16_t address, uint8_t data) {
    if (address == 0x4016) {
        shouldPoll = data;
    } else {
        //TODO: Implement JOY2
    }
}

void Controller::setButtonPressed(int button) {
    shifter |= 1 << button;
}
