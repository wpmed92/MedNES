# MedNES
MedNES is a cycle-accurate NES emulator written in C++.
Supported mappers: NROM(0), MMC1(1), UNROM(2), CNROM(3)

Try it out in your browser [here](https://wpmed92.github.io)!

## Getting Started ##
These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites ###
* **[GIT](https://git-scm.com)**
* **[libSDL2](https://www.libsdl.org/download-2.0.php)**

### Cloning This Repository ###
1. Open Terminal.
2. Change the current working directory to the location where you want the cloned directory to be made.
3. Type `git clone https://github.com/wpmed92/MedNES.git`
4. Press Enter. *Your local clone will be created*.

**Build**

`make`

**Execute**

`./MedNES -insert <path/to/rom>`

### Screenshots ###

| | | |
|:-------------------------:|:-------------------------:|:-------------------------:|
|![alt text](https://github.com/wpmed92/MedNES/blob/master/Screenshots/Super%20Mario%20Bros.png) Super Mario Bros |  ![alt text](https://github.com/wpmed92/MedNES/blob/master/Screenshots/Donkey-Kong.png) Donkey Kong |![alt text](https://github.com/wpmed92/MedNES/blob/master/Screenshots/Bomberman%20II.png) Bomberman II |
|![alt text](https://github.com/wpmed92/MedNES/blob/master/Screenshots/Pac-Man.png) Pac-Man |![alt text](https://github.com/wpmed92/MedNES/blob/master/Screenshots/Mega%20Man.png) Mega Man |![alt text](https://github.com/wpmed92/MedNES/blob/master/Screenshots/Zelda.png) The Legend of Zelda |

## Projects

Here are some projects that build on top of MedNES:

* **[nes-memoryview](https://github.com/sniklaus/nes-memoryview)**: This project is really interesting. It shows how each byte in the 2kB RAM changes over time. You can associate sprite movements with OAM bytes just by looking at the plots! Also, this is the project that enabled the web port!
* **[doogmorning NES emulator](https://gitee.com/doogmorning/programming-windows-/tree/master/FC%E6%A8%A1%E6%8B%9F%E5%99%A8)**: I came across this NES emulator, which added MedNES to its reference list. Go check it out!


## License
>This program is free software: you can redistribute it and/or modify it under the terms of the MIT License.

>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [MIT License](https://en.wikipedia.org/wiki/MIT_License) for more details.

## Contributing Code
_We'd love to accept your patches_! If you have improvements, send us your pull requests!
