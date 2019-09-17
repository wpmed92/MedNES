//
//  main.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 11..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#include <iostream>
#include "../test/CPUTest.hpp"

int main(int argc, char ** argv) {
    //CPU Tests
    CPUTest cpuTest;
    cpuTest.runTest("/users/wpmed92/Desktop/NES/test/nestest.nes", "/users/wpmed92/Desktop/NES/test/nestest.log");
    
    /*ROM rom;
    rom.open("/users/wpmed92/Desktop/NES/roms/Pac-Man.nes");
    rom.printHeader();
    PPU ppu = PPU(&rom);
    CPU6502 cpu = CPU6502(&rom, &ppu);
    cpu.run();*/
    
    return 0;
}
