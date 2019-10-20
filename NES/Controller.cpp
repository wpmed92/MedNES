//
//  Controller.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2019. 09. 20..
//  Copyright © 2019. Ahmed Harmouche. All rights reserved.
//

#include "Controller.hpp"

uint8_t* Controller::read(uint16_t address) {
    return (address == 0x4016) ? &JOY1 : &JOY2;
}

void Controller::write(uint16_t address, uint8_t data) {
    if (address == 0x4016) {
        JOY1 = data;
    } else {
        JOY2 = data;
    }
}
