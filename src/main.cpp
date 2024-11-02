#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <cxxopts.hpp>
#include <box2d/box2d.h>
#include "Level.h"

// Named constants
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr bool DEFAULT_FULLSCREEN = false;
constexpr float GRAVITY_X = 0.0F;
constexpr float GRAVITY_Y = -10.0F;
constexpr int COLOR_ALPHA = 255;

auto main(int argc, char *argv[]) -> int {
    // Default window size
    int windowWidth = WINDOW_WIDTH;
    int windowHeight = WINDOW_HEIGHT;
    bool fullscreen = DEFAULT_FULLSCREEN;
    std::string assetDir = "assets"; // Default asset directory
    std::string levelName = "test_level"; // Default level name

    try {
        cxxopts::Options options(argv[0], "Platformer Prototype");
        options.add_options()
            ("w,width", "Window width", cxxopts::value<int>(windowWidth)->default_value("800"))
            ("h,height", "Window height", cxxopts::value<int>(windowHeight)->default_value("600"))
            ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>(fullscreen)->default_value("false"))
            ("a,assetDir", "Asset directory", cxxopts::value<std::string>(assetDir)->default_value("assets"))
            ("l,levelName", "Level name", cxxopts::value<std::string>(levelName)->default_value("test_level"))
            ("help", "Print help");

        auto result = options.parse(argc, argv);

        if (result.count("help") != 0U) {
            std::cout << options.help() << std::endl;
            return 0;
        }
    }
    catch (const cxxopts::exceptions::exception &e) {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        return 1;
    }

    // Initialize SDL with video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL_Window with SDL_WINDOW_ALLOW_HIGHDPI flag
    SDL_WindowFlags windowFlags =  SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    SDL_Window *window = SDL_CreateWindow("Platformer Prototype",
                                          windowWidth,
                                          windowHeight,
                                          windowFlags);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create SDL_Renderer with SDL_RENDERER_ACCELERATED and SDL_RENDERER_PRESENTVSYNC flags
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize Box2D World
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{GRAVITY_X, GRAVITY_Y}; // Updated to C++-style initialization
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // Create Level object
    Level level(renderer, worldId, assetDir, windowWidth, windowHeight, 24);

    // Load tilemap
    std::string levelPath = assetDir + "/levels/" + levelName + ".tmj";
    if (!level.loadTilemap(levelPath)) {
        std::cerr << "Failed to load tilemap: " << levelPath << std::endl;
        level.handleErrors();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main game loop
    float timeStep = 1.0F / 60.0F;
    int subStepCount = 4;
    bool running = true;
    while (running) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            // Handle other events (keyboard, mouse, etc.)
        }

        // Update physics
        b2World_Step(worldId, timeStep, subStepCount);

        // Game logic and rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, COLOR_ALPHA); // Clear with black color
        SDL_RenderClear(renderer);

        // Render level
        level.render();

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
