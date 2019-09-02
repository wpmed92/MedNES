//
//  6502.cpp
//  NES
//
//  Created by Ahmed Harmouche on 2018. 03. 12..
//  Copyright © 2018. Ahmed Harmouche. All rights reserved.
//

#include "6502.hpp"
#include <assert.h>

void CPU6502::run(bool testMode) {
    //init program counter = $FFFC, $FFFD
    programCounter = testMode ? 16416 : *read(0xFFFD) * 256 + *read(0xFFFC);
    
    while (true) {
        uint8_t instruction = fetchInstruction();
        executeInstruction(instruction);
        programCounter++;
    }
}

uint8_t CPU6502::fetchInstruction() {
    return *read(programCounter);
}

void CPU6502::irq() {
    uint8_t newLsb = *read(0xFFFE);
    uint8_t newMsb = *read(0xFFFF);
    programCounter = newMsb * 256 + newLsb;
}

uint16_t CPU6502::accumulator_adr() {
    return (uint16_t) accumulator;
}

uint16_t CPU6502::immediate() {
    return ++programCounter;
}

uint16_t CPU6502::zeroPage() {
    uint8_t zeroPage = *read(++programCounter);
    return zeroPage % 256;
}

uint16_t CPU6502::zeroPageX() {
    uint8_t zeroPage = *read(++programCounter);
    return (zeroPage + xRegister) % 256;
}

uint16_t CPU6502::zeroPageY() {
    uint8_t zeroPage = *read(++programCounter);
    return (zeroPage + yRegister) % 256;
}

uint16_t CPU6502::absolute() {
    uint8_t lsb = *read(++programCounter);
    uint8_t msb = *read(++programCounter);
    uint16_t address = msb * 256 + lsb;
    
    return address;
}

uint16_t CPU6502::absoluteY() {
    uint8_t lsb = *read(++programCounter);
    uint8_t msb = *read(++programCounter);
    uint16_t address = msb * 256 + lsb;
    
    return address + yRegister;
}

uint16_t CPU6502::absoluteX() {
    uint8_t lsb = *read(++programCounter);
    uint8_t msb = *read(++programCounter);
    uint16_t address = msb * 256 + lsb;
    
    return address + xRegister;
}

uint16_t CPU6502::indirectX() {
    uint16_t operand = (*read(++programCounter) + xRegister) % 256;
    uint8_t lsb = *read(operand);
    uint8_t msb = *read(operand+1);
    uint16_t address = msb * 256 + lsb;
    
    return address;
}

uint16_t CPU6502::indirectY() {
    uint16_t operand = *read(++programCounter);
    uint8_t lsb = *read(operand);
    uint8_t msb = *read(operand+1);
    uint16_t address = (msb * 256 + lsb) + yRegister;
    
    return address;
}

void CPU6502::executeInstruction(uint8_t instruction) {
    switch (instruction) {
        //ADC
        case 0x69:
            ADC(std::bind(&CPU6502::immediate, this));
            break;
        case 0x65:
            ADC(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x75:
            ADC(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x6D:
            ADC(std::bind(&CPU6502::absolute, this));
            break;
        case 0x7D:
            ADC(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0x79:
            ADC(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0x61:
            ADC(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x71:
            ADC(std::bind(&CPU6502::indirectY, this));
            break;
            
        //AND
        case 0x29:
            AND(std::bind(&CPU6502::immediate, this));
            break;
        case 0x25:
            AND(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x35:
            AND(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x2D:
            AND(std::bind(&CPU6502::absolute, this));
            break;
        case 0x3D:
            AND(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0x39:
            AND(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0x21:
            AND(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x31:
            AND(std::bind(&CPU6502::indirectY, this));
            break;
            
        //ASL
        case 0x0A:
            ASL(nullptr);
            break;
        case 0x06:
            ASL(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x16:
            ASL(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x0E:
            ASL(std::bind(&CPU6502::absolute, this));
            break;
        case 0x1E:
            ASL(std::bind(&CPU6502::absoluteX, this));
            break;
            
        //START BRANCH INSTRUCTIONS, ALL RELATIVE!
        //BCC
        case 0x90:
            BCC();
            break;
            
        //BCS
        case 0xB0:
            BCS();
            break;
            
        //BEQ
        case 0xF0:
            BEQ();
            break;
            
        //BMI
        case 0x30:
            BMI();
            break;
        
        //BNE
        case 0xD0:
            BNE();
            break;
            
        //BPL
        case 0x10:
            BPL();
            break;
            
        //BVC
        case 0x50:
            BVC();
            break;
            
        //BVS
        case 0x70:
            BVS();
            break;
        //END BRANCH INSTRUCTION
        
        //BRK - Force interupt
        case 0x00:
            BRK();
            break;
            
        //CLC
        case 0x18:
            CLC();
            break;
            
        //CLD
        case 0xD8:
            CLD();
            break;
        
        //CLI
        case 0x58:
            CLI();
            break;
            
        //CLV
        case 0xB8:
            CLV();
            break;
            
        //CMP
        case 0xC9:
            CMP(std::bind(&CPU6502::immediate, this));
            break;
        case 0xC5:
            CMP(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xD5:
            CMP(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xCD:
            CMP(std::bind(&CPU6502::absolute, this));
            break;
        case 0xDD:
            CMP(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0xD9:
            CMP(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0xC1:
            CMP(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xD1:
            CMP(std::bind(&CPU6502::indirectY, this));
            break;
        
        //CPX
        case 0xE0:
            CPX(std::bind(&CPU6502::immediate, this));
            break;
        case 0xE4:
            CPX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xEC:
            CPX(std::bind(&CPU6502::absolute, this));
            break;
            
        //CPY
        case 0xC0:
            CPY(std::bind(&CPU6502::immediate, this));
            break;
        case 0xC4:
            CPY(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xCC:
            CPY(std::bind(&CPU6502::absolute, this));
            break;
            
        //DEC
        case 0xC6:
            DEC(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xD6:
            DEC(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xCE:
            DEC(std::bind(&CPU6502::absolute, this));
            break;
        case 0xDE:
            DEC(std::bind(&CPU6502::absoluteX, this));
            break;
            
        //DEX
        case 0xCA:
            DEX();
            break;
            
        //DEY
        case 0x88:
            DEY();
            break;
            
        //EOR
        case 0x49:
            EOR(std::bind(&CPU6502::immediate, this));
            break;
        case 0x45:
            EOR(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x55:
            EOR(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x4D:
            EOR(std::bind(&CPU6502::absolute, this));
            break;
        case 0x5D:
            EOR(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0x59:
            EOR(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0x41:
            EOR(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x51:
            EOR(std::bind(&CPU6502::indirectY, this));
            break;
            
        //INC
        case 0xE6:
            INC(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xF6:
            INC(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xEE:
            INC(std::bind(&CPU6502::absolute, this));
            break;
        case 0xFE:
            INC(std::bind(&CPU6502::absoluteX, this));
            break;
            
        //INX
        case 0xE8:
            INX();
            break;
            
        //INY
        case 0xC8:
            INY();
            break;
            
        //JMP
        case 0x4C:
            JMP(std::bind(&CPU6502::absolute, this));
            break;
        case 0x6C:
            JMP(nullptr); //indirect
            break;
            
        //JSR
        case 0x20:
            JSR(std::bind(&CPU6502::absolute, this));
            break;
            
        //LDA
        case 0xA9:
            LDA(std::bind(&CPU6502::immediate, this));
            break;
        case 0xA5:
            LDA(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xB5:
            LDA(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xAD:
            LDA(std::bind(&CPU6502::absolute, this));
            break;
        case 0xBD:
            LDA(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0xB9:
            LDA(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0xA1:
            LDA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xB1:
            LDA(std::bind(&CPU6502::indirectY, this));
            break;
            
        //LDX
        case 0xA2:
            LDX(std::bind(&CPU6502::immediate, this));
            break;
        case 0xA6:
            LDX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xB6:
            LDX(std::bind(&CPU6502::zeroPageY, this));
            break;
        case 0xAE:
            LDX(std::bind(&CPU6502::absolute, this));
            break;
        case 0xBE:
            LDX(std::bind(&CPU6502::absoluteY, this));
            break;
            
        //LDY
        case 0xA0:
            LDY(std::bind(&CPU6502::immediate, this));
            break;
        case 0xA4:
            LDY(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xB4:
            LDY(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xAC:
            LDY(std::bind(&CPU6502::absolute, this));
            break;
        case 0xBC:
            LDY(std::bind(&CPU6502::absoluteX, this));
            break;
        
        //LSR
        //TODO
            
        //NOP
        case 0xEA:
            NOP();
            break;
        
        //ORA
        //TODO
            
        //PHA
        case 0x48:
            PHA();
            break;
            
        //PHP
        case 0x08:
            PHP();
            break;
            
        //PLA
        case 0x68:
            PLA();
            break;
            
        //PLP
        case 0x28:
            PLP();
            break;
            
        //ROL
        case 0x2A:
            ROL(nullptr);
            break;
        case 0x26:
            ROL(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x36:
            ROL(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x2E:
            ROL(std::bind(&CPU6502::absolute, this));
            break;
        case 0x3E:
            ROL(std::bind(&CPU6502::absoluteX, this));
            break;
            
        //ROR
        case 0x6A:
            ROR(nullptr);
            break;
        case 0x66:
            ROR(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x76:
            ROR(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x6E:
            ROR(std::bind(&CPU6502::absolute, this));
            break;
        case 0x7E:
            ROR(std::bind(&CPU6502::absoluteX, this));
            break;
            
        //RTI
        case 0x40:
            RTI();
            break;
            
        //RTS
        case 0x60:
            RTS();
            break;
            
        //SBC
        case 0xE9:
            SBC(std::bind(&CPU6502::immediate, this));
            break;
        case 0xE5:
            SBC(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xF5:
            SBC(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xED:
            SBC(std::bind(&CPU6502::absolute, this));
            break;
        case 0xFD:
            SBC(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0xF9:
            SBC(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0xE1:
            SBC(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xF1:
            SBC(std::bind(&CPU6502::indirectY, this));
            break;
            
        //SEC
        case 0x38:
            SEC();
            break;
            
        //SED
        case 0xF8:
            SED();
            break;
            
        //SEI
        case 0x78:
            SEI();
            break;
            
        //STA
        case 0x85:
            STA(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x95:
            STA(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x8D:
            STA(std::bind(&CPU6502::absolute, this));
            break;
        case 0x9D:
            STA(std::bind(&CPU6502::absoluteX, this));
            break;
        case 0x99:
            STA(std::bind(&CPU6502::absoluteY, this));
            break;
        case 0x81:
            STA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x91:
            STA(std::bind(&CPU6502::indirectY, this));
            break;
            
        //STX
        case 0x86:
            STX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x96:
            STX(std::bind(&CPU6502::zeroPageY, this));
            break;
        case 0x8E:
            STX(std::bind(&CPU6502::absolute, this));
            break;
            
        //STY
        case 0x84:
            STY(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x94:
            STY(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x8C:
            STY(std::bind(&CPU6502::absolute, this));
            break;
            
        //TAX
        case 0xAA:
            TAX();
            break;
            
        //TAY
        case 0xA8:
            TAY();
            break;
            
        //TSX
        case 0xBA:
            TSX();
            break;
            
        //TXA
        case 0x8A:
            TXA();
            break;
            
        //TXS
        case 0x9A:
            TXS();
            break;
            
        //TYA
        case 0x98:
            TYA();
            break;
    }
}

uint8_t* CPU6502::memoryAccess(MemoryAccessMode mode, uint16_t address, uint8_t data) {
    uint8_t* readData = nullptr;
    
    if (address >= 0 && address < 8192) {
        //2KB internal RAM
        
        if (mode == MemoryAccessMode::READ) {
            readData = ram->read(address);
        } else {
            ram->write(address, data);
        }
    } else if (address >= 8192 && address < 16384) {
        //PPU registers
        address %= 8;
    } else if (address >= 16384 && address < 16408) {
        //APU I/O registers
    } else if (address >= 16408 && address < 16416) {
        //CPU test mode
    } else if (address >= 16416 && address < 65535) {
        readData = rom->read(address);
    }
    
    return readData;
}

uint8_t* CPU6502::read(uint16_t address) {
    return memoryAccess(MemoryAccessMode::READ, address, 0);
}

void CPU6502::write(uint16_t address, uint8_t data) {
    memoryAccess(MemoryAccessMode::WRITE, address, data);
}

inline void CPU6502::setSRFlag(CPU6502::StatusFlags flag, bool val) {
    if (val) {
        statusRegister |= (1 << flag);
    } else {
        statusRegister ^= (1 << flag);
    }
}

inline void CPU6502::setNegative(bool val) {
    setSRFlag(StatusFlags::NEGATIVE, val);
}

inline void CPU6502::setOverflow(bool val) {
    setSRFlag(StatusFlags::OVERFLOW, val);
}

inline void CPU6502::setBreak(bool val) {
    setSRFlag(StatusFlags::BREAK, val);
}

inline void CPU6502::setDecimal(bool val) {
    setSRFlag(StatusFlags::DECIMAL, val);
}

inline void CPU6502::setInterruptDisable(bool val) {
    setSRFlag(StatusFlags::INTERRUPT, val);
}

inline void CPU6502::setZero(bool val) {
    setSRFlag(StatusFlags::ZERO, val);
}

inline void CPU6502::setCarry(bool val) {
    setSRFlag(StatusFlags::CARRY, val);
}

void CPU6502::pushStack(uint8_t data) {
    write(stackPointer + 256, data);
    stackPointer--;
}

uint8_t CPU6502::popStack() {
    stackPointer++;
    return *read(stackPointer + 256);
    
}

void CPU6502::ADC(std::function<uint16_t()> addressing) {
    uint8_t data = *read(addressing());
    uint8_t carry = statusRegister & 1;
    uint16_t sum = data + accumulator + carry;
    uint8_t bit7 = (accumulator >> 7) & 1;
    
    if (sum - 256 >= 0) {
        sum -= 256;
        setCarry(1);
    }
    
    setNegative(sum > 127);
    setZero(sum == 0);
    setOverflow(bit7 != ((sum >> 7) & 1));
    accumulator = sum;
}

void CPU6502::AND(std::function<uint16_t()> addressing) {
    accumulator &= *read(addressing());
    setNegative(accumulator > 127);
    setZero(accumulator == 0);
}

void CPU6502::ASL(std::function<uint16_t()> addressing) {
    uint8_t* data = nullptr;
    
    if (addressing == nullptr) {
        data = &accumulator;
    } else {
        data = read(addressing());
    }
    
    uint8_t bit7 = (*data >> 7) & 1;
    *data <<= 1;
    setCarry(bit7);
    setNegative(*data > 127);
    setZero(data == 0);
}

void CPU6502::BCC() {
    uint8_t data = 1;
    uint8_t carry = statusRegister & 1;
    
    if (!carry) {
        programCounter += data;
    }
}

void CPU6502::BCS() {
    uint8_t data = 1;
    uint8_t carry = statusRegister & 1;
    
    if (carry) {
        programCounter += data;
    }
}

void CPU6502::BEQ() {
    uint8_t data = 1;
    uint8_t zero = (statusRegister >> 1) & 1;
    
    if (zero) {
        programCounter += data;
    }
}

void CPU6502::BIT() {
    uint8_t data = 1;
    uint8_t result = accumulator & data;
    uint8_t data_bit6 = (data >> 6) & 1;
    uint8_t data_bit7 = (data >> 7) & 1;
    setZero(result == 0);
    setOverflow(data_bit6);
    setNegative(data_bit7);
}

void CPU6502::BMI() {
    uint8_t data = 1;
    uint8_t neg = (statusRegister >> 7) & 1;
    
    if (neg) {
        programCounter += data;
    }
}

void CPU6502::BNE() {
    uint8_t data = 1;
    uint8_t zero = (statusRegister >> 1) & 1;
    
    if (!zero) {
        programCounter += data;
    }
}

void CPU6502::BPL() {
    uint8_t data = 1;
    uint8_t neg = (statusRegister >> 7) & 1;
    
    if (!neg) {
        programCounter += data;
    }
}

void CPU6502::BRK() {
    uint8_t lsb = programCounter & 0xFF;
    uint8_t msb = programCounter >> 8;
    pushStack(lsb);
    pushStack(msb);
    pushStack(statusRegister);
    irq();
    setBreak(1);
}

void CPU6502::BVC() {
    uint8_t data = 1;
    uint8_t overflow = (statusRegister >> 6) & 1;
    
    if (!overflow) {
        programCounter += data;
    }
}

void CPU6502::BVS() {
    uint8_t data = 1;
    uint8_t overflow = (statusRegister >> 6) & 1;
    
    if (overflow) {
        programCounter += data;
    }
}

void CPU6502::CLC() {
    setCarry(0);
}

void CPU6502::CLD() {
    setDecimal(0);
}

void CPU6502::CLI() {
    setInterruptDisable(0);
}

void CPU6502::CLV() {
    setOverflow(0);
}

void CPU6502::CMP(std::function<uint16_t()> addressing) {
    uint8_t data = *read(addressing());
    uint8_t cmp = accumulator - data;
    setCarry(accumulator >= data);
    setZero(accumulator == data);
    setNegative(cmp > 127);
}

void CPU6502::CPX(std::function<uint16_t()> addressing) {
    uint8_t data = *read(addressing());
    uint8_t cmp = xRegister - data;
    setCarry(xRegister >= data);
    setZero(xRegister == data);
    setNegative(cmp > 127);
}

void CPU6502::CPY(std::function<uint16_t()> addressing) {
    uint8_t data = *read(addressing());
    uint8_t cmp = yRegister - data;
    setCarry(yRegister >= data);
    setZero(yRegister == data);
    setNegative(cmp > 127);
}

void CPU6502::DEC(std::function<uint16_t()> addressing) {
    uint8_t* data = read(addressing());
    (*data)--;
    setZero(*data == 0);
    setNegative(*data > 127);
}

void CPU6502::DEX() {
    xRegister--;
    setZero(xRegister == 0);
    setNegative(xRegister > 127);
}

void CPU6502::DEY() {
    yRegister--;
    setZero(yRegister == 0);
    setNegative(yRegister > 127);
}

void CPU6502::EOR(std::function<uint16_t()> addressing) {
    accumulator ^= *read(addressing());
    setZero(accumulator == 0);
    setNegative(accumulator > 127);
}

void CPU6502::INC(std::function<uint16_t()> addressing) {
    uint8_t* data = read(addressing());
    *data = *data + 1;
    setZero(*data == 0);
    setNegative(*data > 127);
}

void CPU6502::INX() {
    xRegister++;
    setZero(xRegister == 0);
    setNegative(xRegister > 127);
}

void CPU6502::INY() {
    yRegister++;
    setZero(yRegister == 0);
    setNegative(yRegister > 127);
}

void CPU6502::JMP(std::function<uint16_t()> addressing) {
    //indirect
    if (addressing == nullptr) {
        uint8_t lsb = *read(programCounter + 1);
        uint8_t msb = *read(programCounter + 2);
        programCounter = msb * 256 + lsb;
    } else {
        programCounter = addressing();
    }
}

void CPU6502::JSR(std::function<uint16_t()> addressing) {
    uint16_t jumpAddress = addressing();
    uint8_t lsb = programCounter & 0xFF;
    uint8_t msb = programCounter >> 8;
    pushStack(lsb);
    pushStack(msb);
    programCounter = jumpAddress;
}

void CPU6502::LDA(std::function<uint16_t()> addressing) {
    accumulator = *read(addressing());
    setZero(accumulator == 0);
    setNegative(accumulator > 127);
}

void CPU6502::LDX(std::function<uint16_t()> addressing) {
    xRegister = *read(addressing());
    setZero(xRegister == 0);
    setNegative(xRegister > 127);
}

void CPU6502::LDY(std::function<uint16_t()> addressing) {
    yRegister = *read(addressing());
    setZero(yRegister == 0);
    setNegative(yRegister > 127);
}

void CPU6502::LSR(std::function<uint16_t()> addressing) {
    uint8_t data = 1;
    uint8_t bit0 = data & 1;
    data >>= 1;
    setCarry(bit0);
    setNegative(data > 127);
    setZero(data == 0);
}

void CPU6502::NOP() {
    //No operation
}

void CPU6502::ORA(std::function<uint16_t()> addressing) {
    uint8_t data = 1;
    accumulator |= data;
    setZero(accumulator == 0);
    setNegative(accumulator > 127);
}

void CPU6502::PHA() {
    pushStack(accumulator);
}

void CPU6502::PHP() {
    pushStack(statusRegister);
}

void CPU6502::PLA() {
    accumulator = popStack();
    setNegative(accumulator > 127);
    setZero(accumulator == 0);
}

void CPU6502::PLP() {
    statusRegister = popStack();
}

void CPU6502::ROL(std::function<uint16_t()> addressing) {
    uint8_t data = 1;
    uint8_t carry = statusRegister & 1;
    uint8_t bit7 = (data >> 7) & 1;
    data <<= 1;
    data |= carry;
    setCarry(bit7);
    setZero(data == 0);
    setNegative(data > 127);
}

void CPU6502::ROR(std::function<uint16_t()> addressing) {
    uint8_t data = 1;
    uint8_t carry = statusRegister & 1;
    uint8_t bit0 = data & 1;
    data >>= 1;
    data |= carry << 7;
    setCarry(bit0);
    setZero(data == 0);
    setNegative(data > 127);
}

void CPU6502::RTI() {
    statusRegister = popStack();
    uint8_t pcMsb = popStack();
    uint8_t pcLsb = popStack();
    programCounter = pcMsb * 256 + pcLsb;
}

void CPU6502::RTS() {
    uint8_t pcMsb = popStack();
    uint8_t pcLsb = popStack();
    programCounter = pcMsb * 256 + pcLsb + 1;
}

void CPU6502::SBC(std::function<uint16_t()> addressing) {
    
}

void CPU6502::SEC() {
    setCarry(1);
}

void CPU6502::SED() {
    setDecimal(1);
}

void CPU6502::SEI() {
    setInterruptDisable(1);
}

void CPU6502::STA(std::function<uint16_t()> addressing) {
    write(addressing(), accumulator);
}

void CPU6502::STX(std::function<uint16_t()> addressing) {
    write(addressing(), xRegister);
}

void CPU6502::STY(std::function<uint16_t()> addressing) {
    write(addressing(), yRegister);
}

void CPU6502::TAX() {
    xRegister = accumulator;
    setZero(xRegister == 0);
    setNegative(xRegister > 127);
}

void CPU6502::TAY() {
    yRegister = accumulator;
    setZero(yRegister == 0);
    setNegative(yRegister > 127);
}

void CPU6502::TSX() {
    xRegister = stackPointer;
    setZero(xRegister == 0);
    setNegative(xRegister > 127);
}

void CPU6502::TXA() {
    accumulator = xRegister;
    setZero(accumulator == 0);
    setNegative(accumulator > 127);
}

void CPU6502::TXS() {
    stackPointer = xRegister;
}

void CPU6502::TYA() {
    accumulator = yRegister;
    setZero(accumulator == 0);
    setNegative(accumulator > 127);
}

//STATUS REGISTER TESTS
void CPU6502::testStatusFlagsSet() {
    //Initially zero
    assert(statusRegister == 0);
    
    //Carry set
    setCarry(1);
    assert(statusRegister == 1);
    
    //Zero set
    setZero(1);
    assert(statusRegister == 3);
    
    //Interrupt disable set
    setInterruptDisable(1);
    assert(statusRegister == 7);
    
    //Decimal set
    setDecimal(1);
    assert(statusRegister == 15);
    
    //Break set
    setBreak(1);
    assert(statusRegister == 31);
    
    //Overflow set
    setOverflow(1);
    assert(statusRegister == 95);
    
    //Negative set
    setNegative(1);
    assert(statusRegister == 223);
}

void CPU6502::testStatusFlagsUnset() {
    //Initially 11011111 = 223
    statusRegister = 223;
    
    //Carry
    setCarry(0);
    assert(statusRegister == 222);
    
    //Zero
    setZero(0);
    assert(statusRegister == 220);
    
    //Interrupt
    setInterruptDisable(0);
    assert(statusRegister == 216);
    
    //Decimal
    setDecimal(0);
    assert(statusRegister == 208);
    
    //Break
    setBreak(0);
    assert(statusRegister == 192);
    
    //Overflow
    setOverflow(0);
    assert(statusRegister == 128);
    
    //Negative
    setNegative(0);
    assert(statusRegister == 0);
}
