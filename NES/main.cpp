#include <iostream>
#include <chrono>
#include <SDL2/SDL.h>
#include "ROM.hpp"
#include "Mapper/Mapper.hpp"
#include "6502.hpp"
#include "PPU.hpp"
#include "Controller.hpp"

int main(int argc, char ** argv) {
    std::string romPath = "";
    std::string COMMAND_LINE_ERROR_MESSAGE = "Use -insert <path/to/rom> to start playing.";

    if (argc < 2) {
        std::cout << COMMAND_LINE_ERROR_MESSAGE << std::endl;
        return 1;
    }

    std::string option = argv[1];

    if (option == "-insert") {
        romPath = argv[2];
    } else {
        std::cout << "Unkown option '" << option << "'. " << COMMAND_LINE_ERROR_MESSAGE << std::endl;
        return 1;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL could not initialize." << SDL_GetError() << std::endl;
    }
    
    SDL_Window *window;
    std::string window_title = "MedNES";
    bool headlessMode = false;

    window = SDL_CreateWindow(
        window_title.c_str(),                  // window title
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
    SDL_Renderer *s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | ((headlessMode) ? 0 : SDL_RENDERER_PRESENTVSYNC)) ;
    
    ROM rom;
    rom.open(romPath);
    rom.printHeader();
    Mapper* mapper = rom.getMapper();
    PPU ppu = PPU(&rom);
    Controller controller;
    CPU6502 cpu = CPU6502(mapper, &ppu, &controller);
    cpu.reset();
    SDL_Texture * texture = SDL_CreateTexture(s, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);

    //For perf
    int nmiCounter = 0;
    float duration = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    
    while (is_running) {
        cpu.step();

        if (ppu.generateFrame) {
            //Poll controller
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        controller.setButtonPressed(event.key.keysym.sym, true);
                        break;
                    case SDL_KEYUP:
                        controller.setButtonPressed(event.key.keysym.sym, false);
                        break;
                    case SDL_QUIT:
                        is_running = false;
                        break;
                    default:
                        break;
                }
            }
            //Measure fps
            nmiCounter++;
            auto t2 = std::chrono::high_resolution_clock::now();
            duration += std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
            t1 = std::chrono::high_resolution_clock::now();
            
            if (nmiCounter == 10) {
                float avgFps = 1000/(duration/nmiCounter);
                std::string fpsTitle = window_title + " (FPS: " + std::to_string((int) avgFps) + ")";
                SDL_SetWindowTitle(window, fpsTitle.c_str());
                nmiCounter = 0;
                duration = 0;
            }
            
            //Draw frame
            ppu.generateFrame = false;
            SDL_RenderSetScale(s, 2, 2);
            SDL_UpdateTexture(texture, NULL, ppu.buffer, 256 * sizeof(Uint32));
            SDL_RenderClear(s);
            SDL_RenderCopy(s, texture, NULL, NULL);
            SDL_RenderPresent(s);
        }
    }

    SDL_Delay(3000);

    SDL_DestroyWindow(window);
    
    return 0;
}
