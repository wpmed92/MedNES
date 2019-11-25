#include "MapperSelector.hpp"

Mapper* MapperSelector::selectMapper(int numMapper, ROM* rom) {
  switch (numMapper) {
    case 0:
      return new NROM(rom);
      break;

    case 2:
      return new UnROM(rom);
      break;

    default:
      //Unsupported mapper:
      return NULL;
      break;
  }
}