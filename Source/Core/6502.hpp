#pragma once

#include <stdio.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

#include "Common/Typedefs.hpp"
#include "Controller.hpp"
#include "Mapper/Mapper.hpp"
#include "PPU.hpp"
#include "RAM.hpp"

namespace MedNES {

struct ExecutionState {
    u8 accumulator;
    u8 xRegister;
    u8 yRegister;
    u16 programCounter;
    u8 stackPointer;
    u8 statusRegister;
    int cycle;
};

class CPU6502 {
    enum MemoryAccessMode {
        READ,
        WRITE
    };

    enum StatusFlags {
        NEGATIVE = 7,
        OVERFLO = 6,
        BREAK5 = 5,
        BREAK4 = 4,
        DECIMAL = 3,
        INTERRUPT = 2,
        ZERO = 1,
        CARRY = 0
    };

   public:
    CPU6502(Mapper *mapper, PPU *ppu, Controller *controller) : mapper(mapper), ppu(ppu), controller(controller){};
    u8 fetchInstruction();
    void executeInstruction(u8 instruction);
    u8 memoryAccess(MemoryAccessMode mode, u16 address, u8 data);
    u8 read(u16 address);
    void write(u16 address, u8 data);
    void run();
    void step();
    void reset();
    void setProgramCounter(u16 pc);
    ExecutionState *getExecutionState();

   private:
    //Arithmetic
    u8 accumulator = 0;
    u8 xRegister = 0;
    u8 yRegister = 0;

    //Other
    u16 programCounter = 0;
    u8 stackPointer = 0xFD;
    u8 statusRegister = 0x24;

    int cycle = 7;

    //Devices
    RAM ram;
    Mapper *mapper;
    PPU *ppu;
    Controller *controller;

    std::stringstream execLog;

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
    inline void irq();
    inline void NMI();

    inline void LOG_EXEC(u8 instr);

    inline void LOG_PC();

    inline void LOG_CPU_STATE();

    inline void PRINT_LOG();

    inline void tick();

    //stack
    void pushStack(u8);

    u8 popStack();

    //addressing
    u16 immediate();

    u16 zeroPage();

    u16 zeroPageX();

    u16 zeroPageY();

    u16 absolute();

    u16 absoluteX(bool);

    u16 absoluteY(bool);

    u16 indirectX();

    u16 indirectY(bool);

    u16 relative();

    void ADC(std::function<u16()>);

    void ADC(u8);  //for RRA

    //And with accumulator
    void AND(std::function<u16()>);

    void AND(u8);  //for RLA

    //Arithmetic shift left
    void ASL(std::function<u16()>);

    u8 ASL_val(u8);  //for SLO

    //Branch on carry clear
    void BCC(std::function<u16()>);

    //branch on carry set
    void BCS(std::function<u16()>);

    //branch on equal (zero set)
    void BEQ(std::function<u16()>);

    //Bit test
    void BIT(std::function<u16()>);

    //Branch on minus (negative set)
    void BMI(std::function<u16()>);

    //Branch on not equal (zero clear)
    void BNE(std::function<u16()>);

    //Branch on plus (negative clear)
    void BPL(std::function<u16()>);

    //Interrupt
    void BRK();

    //Branch on overflow clear
    void BVC(std::function<u16()>);

    //Branch on overflow set
    void BVS(std::function<u16()>);

    //Clear carry
    void CLC();

    //Clear decimal
    void CLD();

    //clear interrupt disable
    void CLI();

    //Clear overflow
    void CLV();

    //Compare (with accumulator}
    void CMP(std::function<u16()>);

    void CMP(u8);  //for DCP

    //Compare with X
    void CPX(std::function<u16()>);

    //Compare with Y
    void CPY(std::function<u16()>);

    //Decrement
    void DEC(std::function<u16()>);

    u8 DEC(u8);  //for DCP

    //decrement X
    void DEX();

    //decrement Y
    void DEY();

    //Exclusive or (with accumulator)
    void EOR(std::function<u16()>);

    void EOR(u8);  //for SRE

    //Increment
    void INC(std::function<u16()>);

    u8 INC(u8);  //for ISB

    //Increment X
    void INX();

    //Increment Y
    void INY();

    //Jump
    void JMP(std::function<u16()>);

    //Jump subroutine
    void JSR(std::function<u16()>);

    //Load accumulator
    void LDA(std::function<u16()>);

    void LDA(u8);  //for LAX

    //Load X
    void LDX(std::function<u16()>);

    void LDX(u8);  //for LAX

    //Load Y
    void LDY(std::function<u16()>);

    //Logical shift right
    void LSR(std::function<u16()>);

    u8 LSR_val(u8);  //for SRE

    //Or with accumulator
    void ORA(std::function<u16()>);

    void ORA(u8);  //for SLO

    //Push accumulator
    void PHA();

    //Push processor status (SR)
    void PHP();

    //Pull accumulator
    void PLA();

    //Pull processor status (SR)
    void PLP();

    //Rotate left
    void ROL(std::function<u16()>);

    u8 ROL_val(u8);

    //Rotate right
    void ROR(std::function<u16()>);

    u8 ROR_val(u8);  //for RRA

    //Return from interrupt
    void RTI();

    //Return from subroutine
    void RTS();

    //Subtract with carrz
    void SBC(std::function<u16()>);

    void SBC(u8);  //for ISB

    //Set carry
    void SEC();

    //Set decimal;
    void SED();

    //Set interrupt disable
    void SEI();

    //Store accumulator
    void STA(std::function<u16()>);

    //Store X
    void STX(std::function<u16()>);

    //Store Y
    void STY(std::function<u16()>);

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

    //UNOFFICIAL ONES
    void NOP(std::function<u16()>);

    void LAX(std::function<u16()>);

    void SAX(std::function<u16()>);

    void DCP(std::function<u16()>);

    void ISB(std::function<u16()>);

    void SLO(std::function<u16()>);

    void RLA(std::function<u16()>);

    void RRA(std::function<u16()>);

    void SRE(std::function<u16()>);

    void commonBranchLogic(bool, std::function<u16()>);

    void tickIfToNewPage(u16, u16);

    inline void pushPC();
};

};  //namespace MedNES
