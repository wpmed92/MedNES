#include <iostream>
#include "../test/CPUTest.hpp"
#include <SDL2/SDL.h>

int main(int argc, char ** argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL could not initialize." << SDL_GetError() << std::endl;
    }
    
    SDL_Window *window;

    window = SDL_CreateWindow(
        "MedNES",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        512,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_SHOWN                  // flags - see below
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    
    bool is_running = true;
    
    SDL_Event event;
    // We create a renderer with hardware acceleration, we also present according with the vertical sync refresh.
    SDL_Renderer *s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) ;
    
    ROM rom;
    rom.open("/users/wpmed92/Desktop/NES/roms/Balloon-Fight.nes");
    rom.printHeader();
    PPU ppu = PPU(&rom);
    Controller controller;
    CPU6502 cpu = CPU6502(&rom, &ppu, &controller);
    cpu.startup();
    SDL_Texture * texture = SDL_CreateTexture(s, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);

    int nmiCounter = 0;
    
    while (is_running) {
        cpu.step();
        
        if (ppu.generateFrame) {
            nmiCounter++;
            
            if (nmiCounter == 30) {
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        is_running = false;
                    }
                }
                
                nmiCounter = 0;
            }
            
            ppu.printNametable();
            ppu.generateFrame = false;
            Uint32 * pixels = new Uint32[256 * 240];
            
            for (int i = 0; i < 240; i++) {
                for (int j = 0; j < 256; j++) {
                    uint8_t color = ppu.frame[i*256+j] * 64;
                    pixels[i*256+j] = 255 << 24 | color << 16 | color << 8 | color;
                }
            }
            
            SDL_RenderSetScale(s, 2, 2);
            SDL_UpdateTexture(texture, NULL, pixels, 256 * sizeof(Uint32));
            SDL_RenderClear(s);
            SDL_RenderCopy(s, texture, NULL, NULL);
            SDL_RenderPresent(s);
            delete[] pixels;
        }
    }

    SDL_Delay(3000);

    SDL_DestroyWindow(window);
    /*CPUTest cpuTest;
    cpuTest.runTest("/users/wpmed92/Desktop/NES/test/nestest.nes", "/users/wpmed92/Desktop/NES/test/nestest.log");*/
    
    return 0;
}
