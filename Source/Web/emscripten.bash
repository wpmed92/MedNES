#!/bin/bash

~/Desktop/emsdk/emsdk activate latest

source ~/Desktop/emsdk/emsdk_env.sh
mkdir ./build

emcc -O3 -std=c++14 -I../Core -c -o ./build/6502.o ../Core/6502.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/Controller.o ../Core/Controller.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/PPU.o ../Core/PPU.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/RAM.o ../Core/RAM.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/ROM.o ../Core/ROM.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/CNROM.o ../Core/Mapper/CNROM.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/Mapper.o ../Core/Mapper/Mapper.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/NROM.o ../Core/Mapper/NROM.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/UnROM.o ../Core/Mapper/UnROM.cpp
emcc -O3 -std=c++14 -I../Core -c -o ./build/MMC1.o ../Core/Mapper/MMC1.cpp

emcc -O3 -std=c++14 -I../Core -c -o ./build/Emscripten.o ./Emscripten.cpp

emcc -O3 -std=c++14 --memory-init-file 0 -s WASM=1 --bind -s MODULARIZE=1 -s EXPORT_NAME="'Emscripten'" -s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap', 'FS', '_init', '_render', '_key']" ./build/*.o -o emscripten.js

rm -r ./build
