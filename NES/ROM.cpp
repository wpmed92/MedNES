#include "ROM.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Mapper/NROM.hpp"
#include "Mapper/UnROM.hpp"

void ROM::open(std::string filePath ) {
    std::ifstream in(filePath, std::ios::binary);
    
    //Read header
    in.read(reinterpret_cast<char*>(&header.nes), sizeof(char[4]));
    in.read(reinterpret_cast<char*>(&header.prgIn16kb), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.chrIn8kb), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.flags6), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.flags7), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.prgRamIn8kb), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.flags9), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.flags10), sizeof(u8));
    in.read(reinterpret_cast<char*>(&header.zeros), sizeof(char[5]));
    
    trainer.reserve(512);
    int prgSize = header.prgIn16kb * 16384;
    int chrSize = header.chrIn8kb * 8192;
    prgCode.resize(prgSize);
    chrData.resize(chrSize);
    
    mirroring = header.flags6 & 1;
    
    //If trainer present
    if ((header.flags6 >> 2) & 1) {
        in.read((char *)trainer.data(), 512);
    }
    
    in.read((char *)prgCode.data(), header.prgIn16kb * 16384);
    
    if (header.chrIn8kb > 0) {
        in.read((char *)chrData.data(), header.chrIn8kb * 8192);
    }
}

void ROM::printHeader() {
    std::cout << "<<Header>>" << "\n";
    std::cout << "Signature: " << header.nes << "\n";
    std::cout << "PRG ROM (program code) size: " << (int)header.prgIn16kb << " x 16kb \n";
    std::cout << "CHR ROM (graphical data) size: " << (int)header.chrIn8kb << " x 8kb \n";
    mapperNum = ((header.flags6 & 0xF0) >> 4) | (header.flags7 & 0xF0);
    std::bitset<8> flags6Bits(header.flags6);
    std::bitset<8> flags7Bits(header.flags7);
    std::cout << "Flags 6: " << flags6Bits << "\n";
    std::cout << "Flags 7: " << flags7Bits << "\n";
}

void ROM::loadTestProgramcode(std::vector<u8> code) {
    prgCode.insert(prgCode.end(), code.begin(), code.end());
}

int ROM::getMirroring() {
    return mirroring;
}

Mapper* ROM::getMapper() {
  switch (mapperNum) {
    case 0:
      return new NROM(prgCode);
      break;

    case 2:
      return new UnROM(prgCode);
      break;

    default:
      //Unsupported mapper:
      return NULL;
      break;
  }
}

//cpu bus
u8* ROM::read(u16 address) {
    address = (address - 0x8000) % prgCode.size();
    return &prgCode[address];
}

void ROM::write(u16 address, u8 data) {
    //EXCEPTION: READONLY
}

//ppu bus
u8 ROM::ppuread(u16 address) {
    if (header.chrIn8kb == 0) {
        return chrRAM[address];
    } else {
        return chrData[address];
    }
}

void ROM::ppuwrite(u16 address, u8 data) {
    if (header.chrIn8kb == 0) {
        chrRAM[address] = data;
    }
}
