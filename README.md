# MedNES
MedNES is a cycle-accurate NES emulator written in C++.
Supported mappers: NROM(0), UNROM(2), CNROM(3)

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

**Build with make**

`make`

**Build with CMake** 

```sh
mkdir build && cd build
cmake ../
make
```

**Execute**

`./MedNES -insert <path/to/rom>`

### Screenshots ###

| | | |
|:-------------------------:|:-------------------------:|:-------------------------:|
|![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Super%20Mario%20Bros.png) Super Mario Bros |  ![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Donkey-Kong.png) Donkey Kong |![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Bomberman.png) Bomberman |
|![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Pac-Man.png) Pac-Man |  ![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Price%20of%20Persia.png) Prince of Persia |![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Mega%20Man.png) Mega Man |

### License ###
>This program is free software: you can redistribute it and/or modify it under the terms of the MIT License.

>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [MIT License](https://en.wikipedia.org/wiki/MIT_License) for more details.

### Contributing Code ###
_We'd love to accept your patches_! If you have improvements, send us your pull requests!
