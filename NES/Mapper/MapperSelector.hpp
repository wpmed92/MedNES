#pragma once
#include "Mapper.hpp"
#include "NROM.hpp"
#include "UnROM.hpp"

class MapperSelector {
  public:
    static Mapper* selectMapper(int mapperNum, ROM* rom);
};