# MedNES
MedNES is a cycle-accurate NES emulator written in C++.
Supported mappers: NROM(0), UNROM(2)

## Getting Started ##
These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites ###
* **[GIT](https://git-scm.com)**

_For Debian, Ubuntu, Linux Mint and Elementary OS_
```bash
sudo apt-get install git
```

_For ArchLinux and Manjaro_
```bash
sudo pacman -S git
```

_For RHEL, Fedora and CentOS_
```bash
sudo yum install git
```

* **[libSDL2](https://www.libsdl.org/download-2.0.php)**

_For Debian, Ubuntu, Linux Mint and Elementary OS_
```bash
sudo apt-get install libsdl2-2.0-0 libsdl2-dev
```

_For ArchLinux and Manjaro_
```bash
sudo pacman -S libsdl2-2.0-0 libsdl2-dev
```

_For RHEL, Fedora and CentOS_
```bash
sudo yum install SDL2-devel
```

### Cloning This Repository ###
1. Open Terminal.
2. Change the current working directory to the location where you want the cloned directory to be made.
3. Type `git clone https://github.com/wpmed92/MedNES.git`
4. Press Enter. *Your local clone will be created*.

> **Build**
>
> `make`

> **Execute**
>
> `./MedNES -insert <path/to/rom>`

### Screenshots ###

| | | |
|:-------------------------:|:-------------------------:|:-------------------------:|
|![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Super%20Mario%20Bros.png) Super Mario Bros |  ![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Donkey%20Kong.png) Donkey Kong |![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Bomberman.png) Bomberman |
|![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Pac-Man.png) Pac-Man |  ![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Price%20of%20Persia.png) Prince of Persia |![alt text](https://github.com/wpmed92/MedNES/blob/master/screenshots/Mega%20Man.png) Mega Man |

### License ###
>This program is free software: you can redistribute it and/or modify it under the terms of the MIT License.

>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [MIT License](https://en.wikipedia.org/wiki/MIT_License) for more details.

### Contributing Code ###
_We'd love to accept your patches_! If you have improvements, send us your pull requests!