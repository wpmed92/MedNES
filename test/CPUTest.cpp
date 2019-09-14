//
//  CPUTest.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2019. 09. 06..
//  Copyright © 2019. Ahmed Harmouche. All rights reserved.
//

#include "CPUTest.hpp"
#include <fstream>
#include <iomanip>

//C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0,  0 CYC:7
ExecutionState* CPUTest::parseExecutionStateFromLogLine(std::string line) {
    ExecutionState* expectedState = new ExecutionState();
    std::stringstream lexerStream;
    
    //pc
    for (int i = 0; i < 4; i++) {
        lexerStream << line[i];
    }
    
    char * p;
    expectedState->programCounter = (uint16_t) strtol(lexerStream.str().c_str(), & p, 16);
    lexerStream.str("");
    
    //accumulator
    lexerStream << line[50] << line[51];
    expectedState->accumulator = (uint8_t) strtol(lexerStream.str().c_str(), & p, 16);
    lexerStream.str("");
    
    //xregister
    lexerStream << line[55] << line[56];
    expectedState->xRegister = (uint8_t) strtol(lexerStream.str().c_str(), & p, 16);;
    lexerStream.str("");
    
    //yregister
    lexerStream << line[60] << line[61];
    expectedState->yRegister = (uint8_t) strtol(lexerStream.str().c_str(), & p, 16);;
    lexerStream.str("");
    
    //statusregister
    lexerStream << line[65] << line[66];
    expectedState->statusRegister = (uint8_t) strtol(lexerStream.str().c_str(), & p, 16);
    lexerStream.str("");
    
    //stackpointer
    lexerStream << line[71] << line[72];
    expectedState->stackPointer = (uint8_t) strtol(lexerStream.str().c_str(), & p, 16);;
    lexerStream.str("");
    
    return expectedState;
}

void CPUTest::runTest(std::string testROMPath, std::string testLogPath) {
    ROM rom;
    rom.open(testROMPath);
    CPU6502 cpu = CPU6502(&rom, nullptr);
    cpu.setProgramCounter(0xC000);
    
    ExecutionState* expectedExecutionState;
    ExecutionState* actualExecutionState;

    std::string logLine;
    std::ifstream logFile (testLogPath);
    logFile >> std::noskipws;
    
    if (logFile.is_open()) {
        while (getline(logFile, logLine)) {
            expectedExecutionState = parseExecutionStateFromLogLine(logLine);
            actualExecutionState = cpu.getExecutionState();
            
            assert(("Programcounter is incorrect!", actualExecutionState->programCounter == expectedExecutionState->programCounter));
            assert(("Accumulator is incorrect!", actualExecutionState->accumulator == expectedExecutionState->accumulator));
            assert(("xRegister is incorrect!", actualExecutionState->xRegister == expectedExecutionState->xRegister));
            assert(("yRegister is incorrect!", actualExecutionState->yRegister == expectedExecutionState->yRegister));
            assert(("statusRegister is incorrect!", actualExecutionState->statusRegister == expectedExecutionState->statusRegister));
            assert(("stackpointer is incorrect!", actualExecutionState->stackPointer == expectedExecutionState->stackPointer));
            cpu.step();
            
            delete expectedExecutionState;
            delete actualExecutionState;
        }
        
        std::cout << testROMPath << " test PASSED!\n";
        
        logFile.close();
    }

}
