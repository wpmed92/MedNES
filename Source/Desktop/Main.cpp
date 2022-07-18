#include <SDL.h>

#include <chrono>
#include <iostream>
#include <map>

#include "../Core/6502.hpp"
#include "../Core/Controller.hpp"
#include "../Core/Mapper/Mapper.hpp"
#include "../Core/PPU.hpp"
#include "../Core/ROM.hpp"

int main(int argc, char **argv) {
    std::string romPath = "";
    std::string COMMAND_LINE_ERROR_MESSAGE = "Use -insert <path/to/rom> to start playing.";
    bool fullscreen = false;

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

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cout << "SDL could not initialize." << SDL_GetError() << std::endl;
    }

    SDL_GameController *con = nullptr;

    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            con = SDL_GameControllerOpen(i);
            std::cout << "Controller detected.";
            break;
        }
    }

    //TODO: Refactor me
    std::map<int, int> map;
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_A, SDLK_a));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_B, SDLK_b));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_START, SDLK_RETURN));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_UP, SDLK_UP));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDLK_DOWN));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDLK_LEFT));
    map.insert(std::pair<int, int>(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDLK_RIGHT));

    SDL_Window *window;
    std::string window_title = "MedNES";
    bool headlessMode = false;

    window = SDL_CreateWindow(
        window_title.c_str(),     // window title
        SDL_WINDOWPOS_UNDEFINED,  // initial x position
        SDL_WINDOWPOS_UNDEFINED,  // initial y position
        512,                      // width, in pixels
        480,                      // height, in pixels
        SDL_WINDOW_SHOWN          // flags - see below
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    bool is_running = true;

    SDL_Event event;
    // We create a renderer with hardware acceleration, we also present according with the vertical sync refresh.
    SDL_Renderer *s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | ((headlessMode) ? 0 : SDL_RENDERER_PRESENTVSYNC));

    MedNES::ROM rom;
    rom.open(romPath);
    rom.printHeader();
    MedNES::Mapper *mapper = rom.getMapper();

    if (mapper == NULL) {
        std::cout << "Unknown mapper.";
        return 1;
    }

    auto ppu = MedNES::PPU(mapper);
    MedNES::Controller controller;
    auto cpu = MedNES::CPU6502(mapper, &ppu, &controller);
    cpu.reset();
    SDL_Texture *texture = SDL_CreateTexture(s, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);

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
                    case SDL_CONTROLLERBUTTONDOWN:
                        controller.setButtonPressed(map.find(event.cbutton.button)->second, true);
                        break;
                    case SDL_CONTROLLERBUTTONUP:
                        controller.setButtonPressed(map.find(event.cbutton.button)->second, false);
                        break;
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
            duration += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            t1 = std::chrono::high_resolution_clock::now();

            if (nmiCounter == 10) {
                float avgFps = 1000 / (duration / nmiCounter);
                std::string fpsTitle = window_title + " (FPS: " + std::to_string((int)avgFps) + ")";
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
