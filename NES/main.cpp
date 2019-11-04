#include <iostream>
#include "../test/CPUTest.hpp"
#include <SDL2/SDL.h>

int main(int argc, char ** argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL could not initialize." << SDL_GetError() << std::endl;
    }
    
    SDL_Window *window;
    std::string window_title = "MedNES";
    
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
    SDL_Renderer *s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) ;
    
    ROM rom;
    rom.open("/users/wpmed92/Desktop/NES/roms/Antarctic Adventure.nes");
    rom.printHeader();
    PPU ppu = PPU(&rom);
    Controller controller;
    CPU6502 cpu = CPU6502(&rom, &ppu, &controller);
    cpu.startup();
    SDL_Texture * texture = SDL_CreateTexture(s, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);

    //For perf
    int nmiCounter = 0;
    float duration = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    
    while (is_running) {
        cpu.step();
        
        if (ppu.generateFrame) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym) {
                            case SDLK_a:
                                controller.setButtonPressed(0);
                                break;
                            case SDLK_b:
                                controller.setButtonPressed(1);
                                break;
                            case SDLK_SPACE:
                                controller.setButtonPressed(2);
                                break;
                            case SDLK_RETURN:
                                controller.setButtonPressed(3);
                                break;
                            case SDLK_UP:
                                controller.setButtonPressed(4);
                                break;
                            case SDLK_DOWN:
                                controller.setButtonPressed(5);
                                break;
                            case SDLK_LEFT:
                                controller.setButtonPressed(6);
                                break;
                            case SDLK_RIGHT:
                                controller.setButtonPressed(7);
                                break;
                            default:
                                break;
                        }
                        break;
                    //SDL_QUIT event (window close)
                    case SDL_QUIT:
                        is_running = false;
                        break;

                    default:
                        break;
                }
            }
            
            nmiCounter++;
            auto t2 = std::chrono::high_resolution_clock::now();
            duration += std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
            t1 = std::chrono::high_resolution_clock::now();
            
            if (nmiCounter == 60) {
                float avgFps = 1000/(duration/nmiCounter);
                std::string fpsTitle = window_title + " (FPS: " + std::to_string((int) avgFps) + ")";
                SDL_SetWindowTitle(window, fpsTitle.c_str());
                nmiCounter = 0;
                duration = 0;
            }
            
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
    
    return 0;
}
