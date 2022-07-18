#include "6502.hpp"

#include <assert.h>

namespace MedNES {

void CPU6502::step() {
    if (ppu->genNMI()) {
        NMI();
        cycle = 0;
    }

    u8 instruction = fetchInstruction();
    executeInstruction(instruction);
    programCounter++;
}

inline void CPU6502::tick() {
    ppu->tick();
    ppu->tick();
    ppu->tick();
    ++cycle;
}

ExecutionState *CPU6502::getExecutionState() {
    ExecutionState *execState = new ExecutionState();

    execState->accumulator = accumulator;
    execState->xRegister = xRegister;
    execState->yRegister = yRegister;
    execState->statusRegister = statusRegister;
    execState->programCounter = programCounter;
    execState->stackPointer = stackPointer;
    execState->cycle = cycle;

    return execState;
}

void CPU6502::setProgramCounter(u16 pc) {
    programCounter = pc;
}

inline void CPU6502::LOG_EXEC(u8 instr) {
    execLog << std::hex << static_cast<int>(instr) << " ";
}

inline void CPU6502::LOG_PC() {
    u8 lsb = programCounter & 0xFF;
    u8 msb = programCounter >> 8;
    u16 pc = msb * 256 + lsb;
    execLog << std::hex << static_cast<int>(pc) << " ";
}

inline void CPU6502::LOG_CPU_STATE() {
    execLog << "   A:" << std::hex << static_cast<int>(accumulator) << " X:" << std::hex << static_cast<int>(xRegister) << " Y:" << std::hex << static_cast<int>(yRegister) << " P:" << std::hex << static_cast<int>(statusRegister) << " SP:" << std::hex << static_cast<int>(stackPointer);
}

inline void CPU6502::PRINT_LOG() {
    std::cout << execLog.str() << "\n";
    execLog.str("");
}

u8 CPU6502::fetchInstruction() {
    return read(programCounter);
}

inline void CPU6502::pushPC() {
    u8 lsb = programCounter & 0xFF;
    u8 msb = programCounter >> 8;
    pushStack(msb);
    pushStack(lsb);
}

//Interupts
void CPU6502::reset() {
    //init program counter = $FFFC, $FFFD
    programCounter = read(0xFFFD) * 256 + read(0xFFFC);
}

inline void CPU6502::irq() {
    pushPC();
    pushStack(statusRegister);
    u8 lsb = read(0xFFFE);
    u8 msb = read(0xFFFF);
    programCounter = msb * 256 + lsb;
}

inline void CPU6502::NMI() {
    SEI();
    pushPC();
    pushStack(statusRegister);
    u8 lsb = read(0xFFFA);
    u8 msb = read(0xFFFB);
    programCounter = msb * 256 + lsb;
    tick();
}

u16 CPU6502::immediate() {
    return ++programCounter;
}

u16 CPU6502::zeroPage() {
    u8 zeroPage = read(++programCounter);
    return zeroPage % 256;
}

u16 CPU6502::zeroPageX() {
    tick();
    u8 zeroPage = read(++programCounter);
    return (zeroPage + xRegister) % 256;
}

u16 CPU6502::zeroPageY() {
    u8 zeroPage = read(++programCounter);
    return (zeroPage + yRegister) % 256;
}

u16 CPU6502::absolute() {
    u8 lsb = read(++programCounter);
    u8 msb = read(++programCounter);
    u16 address = msb * 256 + lsb;

    return address;
}

u16 CPU6502::absoluteY(bool extraTick) {
    u8 lsb = read(++programCounter);
    u8 msb = read(++programCounter);
    u16 address = msb * 256 + lsb;

    if (extraTick) {
        tickIfToNewPage(address, address + yRegister);
    }

    return address + yRegister;
}

u16 CPU6502::absoluteX(bool extraTick) {
    u8 lsb = read(++programCounter);
    u8 msb = read(++programCounter);
    u16 address = msb * 256 + lsb;

    if (extraTick) {
        tickIfToNewPage(address, address + xRegister);
    }

    return address + xRegister;
}

u16 CPU6502::indirectX() {
    tick();
    u16 operand = (read(++programCounter) + xRegister) % 256;
    u8 lsb = read(operand);
    u8 msb = read((operand + 1) % 256);
    u16 address = msb * 256 + lsb;

    return address;
}

u16 CPU6502::indirectY(bool extraTick) {
    u16 operand = read(++programCounter);
    u8 lsb = read(operand);
    u8 msb = read((operand + 1) % 256);
    u16 address = (msb * 256 + lsb);

    if (extraTick) {
        tickIfToNewPage(address, address + yRegister);
    }

    return address + yRegister;
}

u16 CPU6502::relative() {
    int8_t offset = read(++programCounter);

    return programCounter + offset;
}

void CPU6502::tickIfToNewPage(u16 pc, u16 newPc) {
    u16 newPcMSB = newPc >> 8;
    u16 oldPcMSB = pc >> 8;

    if (newPcMSB != oldPcMSB) {
        tick();
    }
}

void CPU6502::executeInstruction(u8 instruction) {
    switch (instruction) {
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
            ADC(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0x79:
            ADC(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0x61:
            ADC(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x71:
            ADC(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

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
            AND(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0x39:
            AND(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0x21:
            AND(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x31:
            AND(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

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
            ASL(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        //START BRANCH INSTRUCTIONS, ALL RELATIVE!
        case 0x90:
            BCC(std::bind(&CPU6502::relative, this));
            break;
        case 0xB0:
            BCS(std::bind(&CPU6502::relative, this));
            break;
        case 0xF0:
            BEQ(std::bind(&CPU6502::relative, this));
            break;
        case 0x30:
            BMI(std::bind(&CPU6502::relative, this));
            break;
        case 0xD0:
            BNE(std::bind(&CPU6502::relative, this));
            break;
        case 0x10:
            BPL(std::bind(&CPU6502::relative, this));
            break;
        case 0x50:
            BVC(std::bind(&CPU6502::relative, this));
            break;
        case 0x70:
            BVS(std::bind(&CPU6502::relative, this));
            break;
            //END BRANCH INSTRUCTION

        case 0x24:
            BIT(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x2C:
            BIT(std::bind(&CPU6502::absolute, this));
            break;

        case 0x00:
            BRK();
            break;

        case 0x18:
            CLC();
            break;
        case 0xD8:
            CLD();
            break;
        case 0x58:
            CLI();
            break;
        case 0xB8:
            CLV();
            break;

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
            CMP(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0xD9:
            CMP(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0xC1:
            CMP(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xD1:
            CMP(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

        case 0xE0:
            CPX(std::bind(&CPU6502::immediate, this));
            break;
        case 0xE4:
            CPX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xEC:
            CPX(std::bind(&CPU6502::absolute, this));
            break;

        case 0xC0:
            CPY(std::bind(&CPU6502::immediate, this));
            break;
        case 0xC4:
            CPY(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xCC:
            CPY(std::bind(&CPU6502::absolute, this));
            break;

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
            DEC(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0xCA:
            DEX();
            break;
        case 0x88:
            DEY();
            break;

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
            EOR(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0x59:
            EOR(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0x41:
            EOR(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x51:
            EOR(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

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
            INC(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0xE8:
            INX();
            break;
        case 0xC8:
            INY();
            break;

        case 0x4C:
            JMP(std::bind(&CPU6502::absolute, this));
            break;
        case 0x6C:
            JMP(nullptr);
            break;

        case 0x20:
            JSR(std::bind(&CPU6502::absolute, this));
            break;

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
            LDA(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0xB9:
            LDA(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0xA1:
            LDA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xB1:
            LDA(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

        case 0xA2:
            LDX(std::bind(&CPU6502::immediate, this));
            break;
        case 0xA6:
            LDX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xB6:
            LDX(std::bind(&CPU6502::zeroPageY, this));
            tick();
            break;
        case 0xAE:
            LDX(std::bind(&CPU6502::absolute, this));
            break;
        case 0xBE:
            LDX(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;

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
            LDY(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;

        case 0x4A:
            LSR(nullptr);
            break;
        case 0x46:
            LSR(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x56:
            LSR(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x4E:
            LSR(std::bind(&CPU6502::absolute, this));
            break;
        case 0x5E:
            LSR(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0xEA:
            NOP(nullptr);
            break;

        case 0x09:
            ORA(std::bind(&CPU6502::immediate, this));
            break;
        case 0x05:
            ORA(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x15:
            ORA(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x0D:
            ORA(std::bind(&CPU6502::absolute, this));
            break;
        case 0x1D:
            ORA(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0x19:
            ORA(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0x01:
            ORA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x11:
            ORA(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

        case 0x48:
            PHA();
            break;
        case 0x08:
            PHP();
            break;
        case 0x68:
            PLA();
            break;
        case 0x28:
            PLP();
            break;

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
            ROL(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

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
            ROR(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0x40:
            RTI();
            break;
        case 0x60:
            RTS();
            break;

        case 0xE9:
        case 0xEB:
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
            SBC(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;
        case 0xF9:
            SBC(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0xE1:
            SBC(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xF1:
            SBC(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;

        case 0x38:
            SEC();
            break;
        case 0xF8:
            SED();
            break;
        case 0x78:
            SEI();
            break;

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
            STA(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;
        case 0x99:
            STA(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x81:
            STA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x91:
            STA(std::bind(&CPU6502::indirectY, this, /*extraTick*/ false));
            tick();
            break;

        case 0x86:
            STX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x96:
            STX(std::bind(&CPU6502::zeroPageY, this));
            tick();
            break;
        case 0x8E:
            STX(std::bind(&CPU6502::absolute, this));
            break;

        case 0x84:
            STY(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x94:
            STY(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x8C:
            STY(std::bind(&CPU6502::absolute, this));
            break;

        case 0xAA:
            TAX();
            break;
        case 0xA8:
            TAY();
            break;
        case 0xBA:
            TSX();
            break;
        case 0x8A:
            TXA();
            break;
        case 0x9A:
            TXS();
            break;
        case 0x98:
            TYA();
            break;

        //UNOFICIAL OPCODES
        case 0x04:
        case 0x44:
        case 0x64:
            NOP(std::bind(&CPU6502::zeroPage, this));
            tick();
            break;

        case 0x0C:
            NOP(std::bind(&CPU6502::absolute, this));
            tick();
            break;

        case 0x14:
        case 0x34:
        case 0x54:
        case 0x74:
        case 0xD4:
        case 0xF4:
            NOP(std::bind(&CPU6502::zeroPageX, this));
            tick();
            break;

        case 0x1A:
        case 0x3A:
        case 0x5A:
        case 0x7A:
        case 0xDA:
        case 0xFA:
            NOP(nullptr);
            break;

        case 0x80:
            NOP(std::bind(&CPU6502::immediate, this));
            tick();
            break;

        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            NOP(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            tick();
            break;

        case 0xA3:
            LAX(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xA7:
            LAX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xAF:
            LAX(std::bind(&CPU6502::absolute, this));
            break;
        case 0xB3:
            LAX(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;
        case 0xB7:
            LAX(std::bind(&CPU6502::zeroPageY, this));
            tick();
            break;
        case 0xBF:
            LAX(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;

        case 0x83:
            SAX(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x87:
            SAX(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x8F:
            SAX(std::bind(&CPU6502::absolute, this));
            break;
        case 0x97:
            SAX(std::bind(&CPU6502::zeroPageY, this));
            tick();
            break;

        case 0xC3:
            DCP(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xC7:
            DCP(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xCF:
            DCP(std::bind(&CPU6502::absolute, this));
            break;
        case 0xD3:
            DCP(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;
        case 0xD7:
            DCP(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xDB:
            DCP(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0xDF:
            DCP(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;

        case 0xE3:
            ISB(std::bind(&CPU6502::indirectX, this));
            break;
        case 0xE7:
            ISB(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0xEF:
            ISB(std::bind(&CPU6502::absolute, this));
            break;
        case 0xF3:
            ISB(std::bind(&CPU6502::indirectY, this, /*extraTick*/ true));
            break;
        case 0xF7:
            ISB(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0xFB:
            ISB(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ true));
            break;
        case 0xFF:
            ISB(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ true));
            break;

        case 0x03:
            SLO(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x07:
            SLO(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x0F:
            SLO(std::bind(&CPU6502::absolute, this));
            break;
        case 0x13:
            SLO(std::bind(&CPU6502::indirectY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x17:
            SLO(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x1B:
            SLO(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x1F:
            SLO(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0x23:
            RLA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x27:
            RLA(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x2F:
            RLA(std::bind(&CPU6502::absolute, this));
            break;
        case 0x33:
            RLA(std::bind(&CPU6502::indirectY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x37:
            RLA(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x3B:
            RLA(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x3F:
            RLA(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0x43:
            SRE(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x47:
            SRE(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x4F:
            SRE(std::bind(&CPU6502::absolute, this));
            break;
        case 0x53:
            SRE(std::bind(&CPU6502::indirectY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x57:
            SRE(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x5B:
            SRE(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x5F:
            SRE(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        case 0x63:
            RRA(std::bind(&CPU6502::indirectX, this));
            break;
        case 0x67:
            RRA(std::bind(&CPU6502::zeroPage, this));
            break;
        case 0x6F:
            RRA(std::bind(&CPU6502::absolute, this));
            break;
        case 0x73:
            RRA(std::bind(&CPU6502::indirectY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x77:
            RRA(std::bind(&CPU6502::zeroPageX, this));
            break;
        case 0x7B:
            RRA(std::bind(&CPU6502::absoluteY, this, /*extraTick*/ false));
            tick();
            break;
        case 0x7F:
            RRA(std::bind(&CPU6502::absoluteX, this, /*extraTick*/ false));
            tick();
            break;

        default:
            std::cout << "Unkown instruction " << instruction;
            programCounter++;
            break;
    }
}

u8 CPU6502::memoryAccess(MemoryAccessMode mode, u16 address, u8 data) {
    u8 readData = 0;

    if (address >= 0 && address < 0x2000) {
        if (mode == MemoryAccessMode::READ) {
            readData = ram.read(address);
        } else {
            ram.write(address, data);
        }
    } else if (address >= 0x2000 && address < 0x4000) {
        if (mode == MemoryAccessMode::READ) {
            readData = ppu->read(address);
        } else {
            ppu->write(address, data);
        }
    } else if (address >= 0x4000 && address < 0x4018) {
        //COPY OAM
        if (address == 0x4014) {
            if (mode == MemoryAccessMode::READ) {
                std::cout << "No read access at 0x4014";
            } else {
                ppu->write(address, data);

                for (int i = 0; i < 256; i++) {
                    tick();
                    ppu->copyOAM(read(data * 256 + i), i);
                }
            }
        } else {
            if (mode == MemoryAccessMode::READ) {
                readData = controller->read(address);
            } else {
                controller->write(address, data);
            }
        }
        //APU I/O registers
    } else if (address >= 0x4018 && address < 0x4020) {
        //CPU test mode
    } else if (address >= 0x6000 && address <= 0xFFFF) {
        if (mode == MemoryAccessMode::READ) {
            readData = mapper->read(address);
        } else {
            mapper->write(address, data);
        }
    }

    tick();

    return readData;
}

u8 CPU6502::read(u16 address) {
    return memoryAccess(MemoryAccessMode::READ, address, 0);
}

void CPU6502::write(u16 address, u8 data) {
    memoryAccess(MemoryAccessMode::WRITE, address, data);
}

inline void CPU6502::setSRFlag(CPU6502::StatusFlags flag, bool val) {
    if (val) {
        statusRegister |= (1 << flag);
    } else {
        statusRegister &= ~(1 << flag);
    }
}

inline void CPU6502::setNegative(bool val) {
    setSRFlag(StatusFlags::NEGATIVE, val);
}

inline void CPU6502::setOverflow(bool val) {
    setSRFlag(StatusFlags::OVERFLO, val);
}

inline void CPU6502::setBreak4(bool val) {
    setSRFlag(StatusFlags::BREAK4, val);
}

inline void CPU6502::setBreak5(bool val) {
    setSRFlag(StatusFlags::BREAK5, val);
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

void CPU6502::pushStack(u8 data) {
    write(stackPointer + 256, data);
    stackPointer--;
}

u8 CPU6502::popStack() {
    stackPointer++;
    return read(stackPointer + 256);
}

void CPU6502::ADC(std::function<u16()> addressing) {
    ADC(read(addressing()));
}

void CPU6502::ADC(u8 data) {
    u8 carry = statusRegister & 1;
    u16 sum = data + accumulator + carry;
    u8 overflow = (accumulator ^ sum) & (data ^ sum) & 0x80;
    setCarry(sum > 0xFF);
    accumulator = sum;
    setNegative(accumulator & 0x80);
    setZero(accumulator == 0);
    setOverflow(overflow);
}

void CPU6502::AND(std::function<u16()> addressing) {
    AND(read(addressing()));
}

void CPU6502::AND(u8 data) {
    accumulator &= data;
    setNegative(accumulator & 0x80);
    setZero(accumulator == 0);
}

void CPU6502::ASL(std::function<u16()> addressing) {
    if (addressing == nullptr) {
        accumulator = ASL_val(accumulator);
        tick();
    } else {
        u16 address = addressing();
        u8 data = ASL_val(read(address));
        write(address, data);
        tick();
    }
}

u8 CPU6502::ASL_val(u8 data) {
    u8 bit7 = data & 0x80;
    data <<= 1;
    setCarry(bit7);
    setNegative(data & 0x80);
    setZero(data == 0);
    return data;
}

void CPU6502::commonBranchLogic(bool expr, std::function<u16()> resolvePC) {
    if (expr) {
        u16 newPC = resolvePC();
        tickIfToNewPage(programCounter + 1, newPC + 1);
        programCounter = newPC;
        tick();
    } else {
        programCounter++;
        tick();
    }
}

void CPU6502::BCC(std::function<u16()> resolvePC) {
    u8 carry = statusRegister & 1;
    commonBranchLogic(!carry, resolvePC);
}

void CPU6502::BCS(std::function<u16()> resolvePC) {
    u8 carry = statusRegister & 1;
    commonBranchLogic(carry, resolvePC);
}

void CPU6502::BEQ(std::function<u16()> resolvePC) {
    u8 zero = (statusRegister >> 1) & 1;
    commonBranchLogic(zero, resolvePC);
}

void CPU6502::BMI(std::function<u16()> resolvePC) {
    u8 neg = (statusRegister >> 7) & 1;
    commonBranchLogic(neg, resolvePC);
}

void CPU6502::BNE(std::function<u16()> resolvePC) {
    u8 zero = (statusRegister >> 1) & 1;
    commonBranchLogic(!zero, resolvePC);
}

void CPU6502::BPL(std::function<u16()> resolvePC) {
    u8 neg = (statusRegister >> 7) & 1;
    commonBranchLogic(!neg, resolvePC);
}

void CPU6502::BVC(std::function<u16()> resolvePC) {
    u8 overflow = (statusRegister >> 6) & 1;
    commonBranchLogic(!overflow, resolvePC);
}

void CPU6502::BVS(std::function<u16()> resolvePC) {
    u8 overflow = (statusRegister >> 6) & 1;
    commonBranchLogic(overflow, resolvePC);
}

void CPU6502::BIT(std::function<u16()> addressing) {
    u8 data = read(addressing());
    u8 result = accumulator & data;
    u8 data_bit6 = (data >> 6) & 1;
    u8 data_bit7 = (data >> 7) & 1;
    setZero(result == 0);
    setOverflow(data_bit6);
    setNegative(data_bit7);
}

void CPU6502::BRK() {
    programCounter++;
    programCounter++;
    pushPC();
    u8 statusRegCpy = statusRegister;
    statusRegCpy |= (1 << 4);
    //statusRegCpy |= (1 << 5);
    pushStack(statusRegCpy);
    u8 lsb = read(0xFFFE);
    u8 msb = read(0xFFFF);
    programCounter = msb * 256 + lsb - 1;
    tick();
}

void CPU6502::CLC() {
    setCarry(0);
    tick();
}

void CPU6502::CLD() {
    setDecimal(0);
    tick();
}

void CPU6502::CLI() {
    setInterruptDisable(0);
    tick();
}

void CPU6502::CLV() {
    setOverflow(0);
    tick();
}

void CPU6502::CMP(std::function<u16()> addressing) {
    CMP(read(addressing()));
}

void CPU6502::CMP(u8 data) {
    u8 cmp = accumulator - data;
    setCarry(accumulator >= data);
    setZero(accumulator == data);
    setNegative(cmp & 0x80);
}

void CPU6502::CPX(std::function<u16()> addressing) {
    u8 data = read(addressing());
    u8 cmp = xRegister - data;
    setCarry(xRegister >= data);
    setZero(xRegister == data);
    setNegative(cmp & 0x80);
}

void CPU6502::CPY(std::function<u16()> addressing) {
    u8 data = read(addressing());
    u8 cmp = yRegister - data;
    setCarry(yRegister >= data);
    setZero(yRegister == data);
    setNegative(cmp & 0x80);
}

void CPU6502::DEC(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = DEC(read(address));
    write(address, data);
}

u8 CPU6502::DEC(u8 data) {
    data--;
    setZero(data == 0);
    setNegative(data & 0x80);
    tick();
    return data;
}

void CPU6502::DEX() {
    xRegister--;
    setZero(xRegister == 0);
    setNegative(xRegister & 0x80);
    tick();
}

void CPU6502::DEY() {
    yRegister--;
    setZero(yRegister == 0);
    setNegative(yRegister & 0x80);
    tick();
}

void CPU6502::EOR(std::function<u16()> addressing) {
    EOR(read(addressing()));
}

void CPU6502::EOR(u8 data) {
    accumulator ^= data;
    setZero(accumulator == 0);
    setNegative(accumulator & 0x80);
}

void CPU6502::INC(std::function<u16()> addressing) {
    u16 address = addressing();
    write(address, INC(read(address)));
}

u8 CPU6502::INC(u8 data) {
    data = data + 1;
    setZero(data == 0);
    setNegative(data & 0x80);
    tick();
    return data;
}

void CPU6502::INX() {
    xRegister++;
    setZero(xRegister == 0);
    setNegative(xRegister & 0x80);
    tick();
}

void CPU6502::INY() {
    yRegister++;
    setZero(yRegister == 0);
    setNegative(yRegister & 0x80);
    tick();
}

void CPU6502::JMP(std::function<u16()> addressing) {
    //indirect
    if (addressing == nullptr) {
        u8 lsb = read(programCounter + 1);
        u8 msb = read(programCounter + 2);
        u16 address = msb * 256 + lsb;
        u8 lsbt = read(address);
        u16 msbAddress = (address & 0xFF) == 0xFF ? address & 0xFF00 : address + 1;
        u8 msbt = read(msbAddress);
        programCounter = msbt * 256 + lsbt - 1;
    } else {
        programCounter = addressing() - 1;
    }
}

void CPU6502::JSR(std::function<u16()> addressing) {
    u16 jumpAddress = addressing();
    u8 lsb = programCounter & 0xFF;
    u8 msb = programCounter >> 8;
    pushStack(msb);
    pushStack(lsb);
    programCounter = jumpAddress - 1;
    tick();
}

void CPU6502::LDA(std::function<u16()> addressing) {
    LDA(read(addressing()));
}

void CPU6502::LDA(u8 data) {
    accumulator = data;
    setZero(accumulator == 0);
    setNegative(accumulator & 0x80);
}

void CPU6502::LDX(u8 data) {
    xRegister = data;
    setZero(xRegister == 0);
    setNegative(xRegister & 0x80);
}

void CPU6502::LDX(std::function<u16()> addressing) {
    LDX(read(addressing()));
}

void CPU6502::LDY(std::function<u16()> addressing) {
    yRegister = read(addressing());
    setZero(yRegister == 0);
    setNegative(yRegister & 0x80);
}

void CPU6502::LSR(std::function<u16()> addressing) {
    if (addressing == nullptr) {
        accumulator = LSR_val(accumulator);
        tick();
    } else {
        u16 address = addressing();
        u8 data = read(address);
        write(address, LSR_val(data));
        tick();
    }
}

u8 CPU6502::LSR_val(u8 data) {
    u8 bit0 = data & 1;
    data >>= 1;
    setCarry(bit0);
    setNegative(data & 0x80);
    setZero(data == 0);
    return data;
}

void CPU6502::NOP(std::function<u16()> addressing) {
    //Unofficial ones have addressing modes.
    if (addressing != nullptr) {
        addressing();
    } else {
        tick();
    }
}

void CPU6502::ORA(std::function<u16()> addressing) {
    ORA(read(addressing()));
}

void CPU6502::ORA(u8 data) {
    accumulator |= data;
    setZero(accumulator == 0);
    setNegative(accumulator & 0x80);
}

void CPU6502::PHA() {
    pushStack(accumulator);
    tick();
}

void CPU6502::PHP() {
    u8 statusRegCpy = statusRegister;
    statusRegCpy |= (1 << 4);
    statusRegCpy |= (1 << 5);
    pushStack(statusRegCpy);
    tick();
}

void CPU6502::PLA() {
    accumulator = popStack();
    setNegative(accumulator & 0x80);
    setZero(accumulator == 0);
    tick();
    tick();
}

void CPU6502::PLP() {
    statusRegister = popStack();
    setBreak4(0);
    setBreak5(1);
    tick();
    tick();
}

void CPU6502::ROL(std::function<u16()> addressing) {
    if (addressing == nullptr) {
        accumulator = ROL_val(accumulator);
        tick();
    } else {
        u16 address = addressing();
        u8 data = ROL_val(read(address));
        write(address, data);
        tick();
    }
}

u8 CPU6502::ROL_val(u8 data) {
    u8 bit7 = data & 0x80;
    data = (data << 1) | (statusRegister & 1);
    setCarry(bit7);
    setZero(data == 0);
    setNegative(data & 0x80);
    return data;
}

void CPU6502::ROR(std::function<u16()> addressing) {
    if (addressing == nullptr) {
        accumulator = ROR_val(accumulator);
        tick();
    } else {
        u16 address = addressing();
        u8 data = ROR_val(read(address));
        write(address, data);
        tick();
    }
}

u8 CPU6502::ROR_val(u8 data) {
    u8 bit0 = data & 1;
    data = (data >> 1) | ((statusRegister & 1) << 7);
    setCarry(bit0);
    setZero(data == 0);
    setNegative(data & 0x80);
    return data;
}

void CPU6502::RTI() {
    statusRegister = popStack();
    setBreak4(0);
    setBreak5(1);
    u8 pcLsb = popStack();
    u8 pcMsb = popStack();
    programCounter = pcMsb * 256 + pcLsb - 1;
    tick();
    tick();
}

void CPU6502::RTS() {
    u8 pcLsb = popStack();
    u8 pcMsb = popStack();
    programCounter = pcMsb * 256 + pcLsb;
    tick();
    tick();
    tick();
}

void CPU6502::SBC(std::function<u16()> addressing) {
    SBC(read(addressing()));
}

void CPU6502::SBC(u8 data) {
    ADC(data ^ 0xFF);
}

void CPU6502::SEC() {
    setCarry(1);
    tick();
}

void CPU6502::SED() {
    setDecimal(1);
    tick();
}

void CPU6502::SEI() {
    setInterruptDisable(1);
    tick();
}

void CPU6502::STA(std::function<u16()> addressing) {
    write(addressing(), accumulator);
}

void CPU6502::STX(std::function<u16()> addressing) {
    write(addressing(), xRegister);
}

void CPU6502::STY(std::function<u16()> addressing) {
    write(addressing(), yRegister);
}

void CPU6502::TAX() {
    xRegister = accumulator;
    setZero(xRegister == 0);
    setNegative(xRegister & 0x80);
    tick();
}

void CPU6502::TAY() {
    yRegister = accumulator;
    setZero(yRegister == 0);
    setNegative(yRegister & 0x80);
    tick();
}

void CPU6502::TSX() {
    xRegister = stackPointer;
    setZero(xRegister == 0);
    setNegative(xRegister & 0x80);
    tick();
}

void CPU6502::TXA() {
    accumulator = xRegister;
    setZero(accumulator == 0);
    setNegative(accumulator & 0x80);
    tick();
}

void CPU6502::TXS() {
    stackPointer = xRegister;
    tick();
}

void CPU6502::TYA() {
    accumulator = yRegister;
    setZero(accumulator == 0);
    setNegative(accumulator & 0x80);
    tick();
}

//UNOFFICIAL OPCODES
//LDA+LDX
void CPU6502::LAX(std::function<u16()> addressing) {
    u8 data = read(addressing());
    LDA(data);
    LDX(data);
}

//STA+acc&x
void CPU6502::SAX(std::function<u16()> addressing) {
    write(addressing(), accumulator & xRegister);
}

//DEC+CMP
void CPU6502::DCP(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = DEC(read(address));
    write(address, data);
    CMP(data);
}

//INC+SBC
void CPU6502::ISB(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = INC(read(address));
    write(address, data);
    SBC(data);
}

//ASL+ORA
void CPU6502::SLO(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = ASL_val(read(address));
    write(address, data);
    ORA(data);
    tick();
}

//ROL+AND
void CPU6502::RLA(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = ROL_val(read(address));
    write(address, data);
    AND(data);
    tick();
}

//LSR+EOR
void CPU6502::SRE(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = LSR_val(read(address));
    write(address, data);
    EOR(data);
    tick();
}

//ROR+ADC
void CPU6502::RRA(std::function<u16()> addressing) {
    u16 address = addressing();
    u8 data = ROR_val(read(address));
    write(address, data);
    ADC(data);
    tick();
}

}  //namespace MedNES
