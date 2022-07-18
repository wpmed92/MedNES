#pragma once

#include <stdio.h>
#include <string>
#include <SDL.h>
#include "INESBus.hpp"
#include "Common/Typedefs.hpp"

namespace MedNES
{

  class Controller : INESBus
  {
    u8 JOY1 = 0;
    u8 JOY2 = 0;
    u8 btnStateLocked = 0;
    u8 btnState = 0;
    bool strobe;

  public:
    //Bus
    u8 read(u16 address);
    void write(u16 address, u8 data);

    //Input
    void setButtonPressed(SDL_Keycode, bool);
  };

}; //namespace MedNES
