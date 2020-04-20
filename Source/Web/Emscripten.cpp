#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <vector>

#define minimum(intA, intB) (intA < intB) ? (intA) : (intB)
#define maximum(intA, intB) (intA > intB) ? (intA) : (intB)

static inline unsigned long long milliseconds() {
    timespec objTimespec = { };

    clock_gettime(CLOCK_MONOTONIC, &objTimespec);

    return (objTimespec.tv_sec * 1000) + (objTimespec.tv_nsec / 1000000);
}

static inline unsigned long long microseconds() {
    timespec objTimespec = { };

    clock_gettime(CLOCK_MONOTONIC, &objTimespec);

    return (objTimespec.tv_sec * 1000000) + (objTimespec.tv_nsec / 1000);
}

#include "emscripten.h"

// ----------------------------------------------------------

#include "6502.hpp"
#include "Controller.hpp"
#include "Mapper/Mapper.hpp"
#include "PPU.hpp"
#include "ROM.hpp"

ROM* objRom = NULL;
Mapper* objMapper = NULL;
PPU* objPpu = NULL;
Controller* objController = NULL;
CPU6502* objCpu = NULL;

// ----------------------------------------------------------

extern "C" bool EMSCRIPTEN_KEEPALIVE init(const char* charRom) {
    if (objRom != NULL) { 
        delete objRom;
        delete objMapper;
        delete objPpu;
        delete objController;
        delete objCpu;
    }

    std::cout << charRom << std::endl;
    objRom = new ROM();
    objRom->open(std::string(charRom));
    objRom->printHeader();

    objMapper = objRom->getMapper();

    if (objMapper == NULL) {
        return false;
    }

    objPpu = new PPU(objMapper);

    objController = new Controller();

    objCpu = new CPU6502(objMapper, objPpu, objController);
    objCpu->reset();

    return true;
}

extern "C" void EMSCRIPTEN_KEEPALIVE render(unsigned char* charPixels) {
    if (objRom == NULL) { 
        return;
    }

    while (objPpu->generateFrame == false) {
        objCpu->step();
    }

    objPpu->generateFrame = false;

    unsigned char* charPpu = (unsigned char*) (objPpu->buffer);

    for (int intY = 0; intY < 480; intY += 1) {
        for (int intX = 0; intX < 512; intX += 1) {
            charPixels[(intY * 512 * 4) + (intX * 4) + 0] = charPpu[((intY / 2) * 256 * 4) + ((intX / 2) * 4) + 2];
            charPixels[(intY * 512 * 4) + (intX * 4) + 1] = charPpu[((intY / 2) * 256 * 4) + ((intX / 2) * 4) + 1];
            charPixels[(intY * 512 * 4) + (intX * 4) + 2] = charPpu[((intY / 2) * 256 * 4) + ((intX / 2) * 4) + 0];
            charPixels[(intY * 512 * 4) + (intX * 4) + 3] = 255;
        }
    }
}

extern "C" void EMSCRIPTEN_KEEPALIVE key(int intState, int intKey) {
    if (objRom == NULL) { 
        return;
    }

    if (intKey == 37) {
        objController->setButtonPressed(SDLK_LEFT, intState);
        
    } else if (intKey == 38) {
        objController->setButtonPressed(SDLK_UP, intState);
        
    } else if (intKey == 39) {
        objController->setButtonPressed(SDLK_RIGHT, intState);
        
    } else if (intKey == 40) {
        objController->setButtonPressed(SDLK_DOWN, intState);
        
    } else if (intKey == 88) {
        objController->setButtonPressed(SDLK_a, intState);
        
    } else if (intKey == 67) {
        objController->setButtonPressed(SDLK_b, intState);
        
    } else if (intKey == 32) {
        objController->setButtonPressed(SDLK_SPACE, intState);
        
    } else if (intKey == 13) {
        objController->setButtonPressed(SDLK_RETURN, intState);
        
    }
}
