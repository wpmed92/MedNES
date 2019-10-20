#include "ROM.hpp"
#include <fstream>
#include <iostream>

void ROM::open(std::string filePath ) {
    std::ifstream in(filePath, std::ios::binary);
    
    //Read header
    in.read(reinterpret_cast<char*>(&header.nes), sizeof(char[4]));
    in.read(reinterpret_cast<char*>(&header.prgIn16kb), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.chrIn8kb), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.flags6), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.flags7), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.prgRamIn8kb), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.flags9), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.flags10), sizeof(uint8_t));
    in.read(reinterpret_cast<char*>(&header.zeros), sizeof(char[5]));
    
    trainer.reserve(512);
    int prgSize = header.prgIn16kb * 16384;
    int chrSize = header.chrIn8kb * 8192;
    prgCode.resize(prgSize);
    chrData.resize(chrSize);
    
    //If trainer present
    if ((header.flags6 >> 2) & 1) {
        in.read((char *)trainer.data(), 512);
    }
    
    in.read((char *)prgCode.data(), header.prgIn16kb * 16384);
    in.read((char *)chrData.data(), header.chrIn8kb * 8192);
}

void ROM::printHeader() {
    std::cout << "<<Header>>" << "\n";
    std::cout << "Signature: " << header.nes << "\n";
    std::cout << "PRG ROM (program code) size: " << (int)header.prgIn16kb << " x 16kb \n";
    std::cout << "CHR ROM (graphical data) size: " << (int)header.chrIn8kb << " x 8kb \n";
    std::bitset<8> flags6Bits(header.flags6);
    std::bitset<8> flags7Bits(header.flags7);
    std::cout << "Flags 6: " << flags6Bits << "\n";
    std::cout << "Flags 7: " << flags7Bits << "\n";
}

void ROM::loadTestProgramcode(std::vector<uint8_t> code) {
    prgCode.insert(prgCode.end(), code.begin(), code.end());
}

//cpu bus
uint8_t* ROM::read(uint16_t address) {
    address = (address - 0x8000) % prgCode.size();
    return &prgCode[address];
}

void ROM::write(uint16_t address, uint8_t data) {
    //EXCEPTION: READONLY
}

//ppu bus
uint8_t ROM::ppuread(uint16_t address) {
    return chrData[address];
}

void ROM::ppuwrite(uint16_t address, uint8_t data) {
    //EXCEPTION: READONLY
}
