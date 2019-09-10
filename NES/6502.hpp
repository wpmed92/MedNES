//
//  6502.hpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 12..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#ifndef _502_hpp
#define _502_hpp

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "RAM.hpp"
#include "ROM.hpp"
#include "PPU.hpp"

struct ExecutionState {
    uint8_t accumulator;
    uint8_t xRegister;
    uint8_t yRegister;
    uint16_t programCounter;
    uint8_t stackPointer;
    uint8_t statusRegister;
    bool isUnofficialOpcode;
    uint8_t unofficialOffset;
};

class CPU6502 {
    
private:
    //Arithmetic
    uint8_t accumulator = 0;
    uint8_t xRegister = 0;
    uint8_t yRegister = 0;
    
    //Other
    uint16_t programCounter = 0;
    uint8_t stackPointer = 0xFD;
    uint8_t statusRegister = 0x24;
    
    //Devices
    RAM* ram;
    ROM* rom;
    PPU* ppu;
    
    std::stringstream execLog;
    
    enum MemoryAccessMode {
        READ,
        WRITE
    };
    
    enum StatusFlags {
        NEGATIVE = 7,
        OVERFLOW = 6,
        BREAK5 = 5,
        BREAK4 = 4,
        DECIMAL = 3,
        INTERRUPT = 2,
        ZERO = 1,
        CARRY = 0
    };
    
    inline void setSRFlag(StatusFlags, bool);
    inline void setNegative(bool);
    inline void setOverflow(bool);
    inline void setBreak4(bool);
    inline void setBreak5(bool);
    inline void setDecimal(bool);
    inline void setInterruptDisable(bool);
    inline void setZero(bool);
    inline void setCarry(bool);
    
    //vectors
    void irq();
    
    void reset();
    
    inline void LOG_EXEC(uint8_t instr);
    
    inline void LOG_PC();
    
    inline void LOG_CPU_STATE();
    
    inline void PRINT_LOG();
    
    //stack
    void pushStack(uint8_t);
    
    uint8_t popStack();
    
    //addressing
    uint16_t accumulator_adr();
    
    uint16_t immediate();
    
    uint16_t zeroPage();
    
    uint16_t zeroPageX();
    
    uint16_t zeroPageY();
    
    uint16_t absolute();
    
    uint16_t absoluteX();
    
    uint16_t absoluteY();
    
    uint16_t indirectX();
    
    uint16_t indirectY();
    
    uint16_t relative();
    
    void ADC(std::function<uint16_t()>);
    
    //And with accumulator
    void AND(std::function<uint16_t()>);
    
    //Arithmetic shift left
    void ASL(std::function<uint16_t()>);
    
    //Branch on carry clear
    void BCC(std::function<uint16_t()>);
    
    //branch on carry set
    void BCS(std::function<uint16_t()>);
    
    //branch on equal (zero set)
    void BEQ(std::function<uint16_t()>);
    
    //Bit test
    void BIT(std::function<uint16_t()>);
    
    //Branch on minus (negative set)
    void BMI(std::function<uint16_t()>);
    
    //Branch on not equal (zero clear)
    void BNE(std::function<uint16_t()>);
    
    //Branch on plus (negative clear)
    void BPL(std::function<uint16_t()>);
    
    //Interrupt
    void BRK();
    
    //Branch on overflow clear
    void BVC(std::function<uint16_t()>);
    
    //Branch on overflow set
    void BVS(std::function<uint16_t()>);
    
    //Clear carry
    void CLC();
    
    //Clear decimal
    void CLD();
    
    //clear interrupt disable
    void CLI();
    
    //Clear overflow
    void CLV();
    
    //Compare (with accumulator}
    void CMP(std::function<uint16_t()>);
    
    //Compare with X
    void CPX(std::function<uint16_t()>);
    
    //Compare with Y
    void CPY(std::function<uint16_t()>);
    
    //Decrement
    void DEC(std::function<uint16_t()>);
    
    //decrement X
    void DEX();
    
    //decrement Y
    void DEY();
    
    //Exclusive or (with accumulator)
    void EOR(std::function<uint16_t()>);
    
    //Increment
    void INC(std::function<uint16_t()>);
    
    //Increment X
    void INX();
    
    //Increment Y
    void INY();
    
    //Jump
    void JMP(std::function<uint16_t()>);
    
    //Jump subroutine
    void JSR(std::function<uint16_t()>);
    
    //Load accumulator
    void LDA(std::function<uint16_t()>);
    
    //Load X
    void LDX(std::function<uint16_t()>);
    
    //Load Y
    void LDY(std::function<uint16_t()>);
    
    //Logical shift right
    void LSR(std::function<uint16_t()>);
    
    //No operation
    void NOP();
    
    //Or with accumulator
    void ORA(std::function<uint16_t()>);
    
    //Push accumulator
    void PHA();
    
    //Push processor status (SR)
    void PHP();
    
    //Pull accumulator
    void PLA();
    
    //Pull processor status (SR)
    void PLP();
    
    //Rotate left
    void ROL(std::function<uint16_t()>);
    
    //Rotate right
    void ROR(std::function<uint16_t()>);
    
    //Return from interrupt
    void RTI();
    
    //Return from subroutine
    void RTS();
    
    //Subtract with carrz
    void SBC(std::function<uint16_t()>);
    
    //Set carry
    void SEC();
    
    //Set decimal;
    void SED();
    
    //Set interrupt disable
    void SEI();
    
    //Store accumulator
    void STA(std::function<uint16_t()>);
    
    //Store X
    void STX(std::function<uint16_t()>);
    
    //Store Y
    void STY(std::function<uint16_t()>);
    
    //Transfer accumulator to X
    void TAX();
    
    //Transfer accumulator to Y
    void TAY();
    
    //Transfer stack pointer to X
    void TSX();
    
    //Transfer X to accumulator
    void TXA();
    
    //Transfer X to stack pointer
    void TXS();
    
    //Transfer Y to accumulator
    void TYA();
    
    
public:
    CPU6502(RAM* ram, ROM* rom, PPU* ppu) : ram(ram), rom(rom), ppu(ppu) {};
    uint8_t fetchInstruction();
    void executeInstruction(uint8_t);
    uint8_t* memoryAccess(MemoryAccessMode, uint16_t, uint8_t);
    uint8_t* read(uint16_t);
    void write(uint16_t, uint8_t);
    void run(int);
    void step();
    void setProgramCounter(uint16_t);
    ExecutionState* getExecutionState();
    void setExecutionState(ExecutionState*);
};

#endif /* _502_hpp */
