//
//  main.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 11..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#include <iostream>
#include "../test/CPUTest.hpp"

int main(int argc, const char * argv[]) {
    //CPU Tests
    CPUTest cpuTest;
    cpuTest.runTest("/users/wpmed92/Desktop/NES/roms/nestest.nes", "/users/wpmed92/Desktop/NES/test/nestest.log");
    
    return 0;
}
