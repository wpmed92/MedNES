#include "ROM.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "Mapper/CNROM.hpp"
#include "Mapper/MMC1.hpp"
#include "Mapper/NROM.hpp"
#include "Mapper/UnROM.hpp"

namespace MedNES {

void ROM::open(std::string filePath) {
    std::ifstream in(filePath, std::ios::binary);

    //Read header
    in.read(reinterpret_cast<char *>(&header.nes), sizeof(char[4]));
    in.read(reinterpret_cast<char *>(&header.prgIn16kb), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.chrIn8kb), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags6), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags7), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.prgRamIn8kb), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags9), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags10), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.zeros), sizeof(char[5]));

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
    } else {
        chrData = std::vector<u8>(8192, 0);
    }
}

void ROM::printHeader() {
    std::cout << "<<Header>>"
              << "\n";
    std::cout << "Signature: " << header.nes << "\n";
    std::cout << "PRG ROM (program code) size: " << (int)header.prgIn16kb << " x 16kb \n";
    std::cout << "CHR ROM (graphical data) size: " << (int)header.chrIn8kb << " x 8kb \n";
    mapperNum = ((header.flags6 & 0xF0) >> 4) | (header.flags7 & 0xF0);
    std::bitset<8> flags6Bits(header.flags6);
    std::bitset<8> flags7Bits(header.flags7);
    std::cout << "Flags 6: " << flags6Bits << "\n";
    std::cout << "Flags 7: " << flags7Bits << "\n";
}

int ROM::getMirroring() {
    return mirroring;
}

Mapper *ROM::getMapper() {
    switch (mapperNum) {
        case 0:
            return new NROM(prgCode, chrData, mirroring);
            break;

        case 1:
            return new MMC1(prgCode, chrData, mirroring);
            break;

        case 2:
            return new UnROM(prgCode, chrData, mirroring);
            break;

        case 3:
            return new CNROM(prgCode, chrData, mirroring);
            break;

        default:
            //Unsupported mapper:
            return NULL;
            break;
    }
}

}  //namespace MedNES
