//
//  main.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 11..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#include <iostream>
#include "6502.hpp"
#include "ROM.hpp"
#include <assert.h>

int main(int argc, const char * argv[]) {
    //CPU TESTS
    /*CPU6502 cpu;
    cpu.testStatusFlagsSet();
    std::cout << "TEST: **Status flags SET** PASSED" << "\n";
    cpu.testStatusFlagsUnset();
    std::cout << "TEST: **Status flags UNSET** PASSED" << "\n";*/
    
    //CPU Tests
    ROM rom;
    std::vector<uint8_t> code { 0x69, 0x10 };
    rom.loadTestProgramcode(code);
    CPU6502 cpu = CPU6502(nullptr, &rom, nullptr);
    cpu.run(true);
    
    return 0;
}
