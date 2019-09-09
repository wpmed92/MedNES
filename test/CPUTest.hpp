//
//  CPUTest.hpp
//  NES
//
//  Created by Ahmed Harmouche on 2019. 09. 06..
//  Copyright © 2019. Ahmed Harmouche. All rights reserved.
//

#ifndef CPUTest_hpp
#define CPUTest_hpp

#include <stdio.h>
#include "6502.hpp"

class CPUTest {
private:
    ExecutionState* parseExecutionStateFromLogLine(std::string);
    
public:
    CPUTest() {};
    void runTest(std::string, std::string);
    
};

#endif /* CPUTest_hpp */
