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
        //Unimplemented
        return &JOY2;
    }
}

void Controller::write(uint16_t address, uint8_t data) {
    if (address == 0x4016) {
        shouldPoll = data;
        JOY1 = data;
    } else {
        JOY2 = data;
    }
}

void Controller::setButtonState(std::string buttons) {
    for (int i = 0; i < buttons.length(); i++) {
        switch(buttons[i]) {
            case 'a':
                shifter |= 1;
                break;
            
            case 'b':
                shifter |= 2;
                break;
                
            case '2': //select
                shifter |= 4;
                break;
                
            case '3': //start
                shifter |= 8;
                break;
                
            case 'u':
                shifter |= 16;
                break;
                
            case 'd':
                shifter |= 32;
                break;
                
            case 'l':
                shifter |= 64;
                break;
                
            case 'r':
                shifter |= 128;
                break;
            
        }
    }
}
                            
void Controller::resetButtonState() {
   shifter = 0;
}
