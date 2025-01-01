#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <cxxopts.hpp>
#include <box2d/box2d.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Level.h"
#include "Character.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "Logging.h"
#include "Config.h"
#include "DeveloperMenu.h"
#include "GameSettingsObserver.h"
#include "Box2DDebugDraw.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

// Named constants
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr bool DEFAULT_FULLSCREEN = false;
constexpr float GRAVITY_X = 0.0F;
constexpr float GRAVITY_Y = -9.8F; // Earth's gravity
constexpr int COLOR_ALPHA = 255;
constexpr float FRAMES_PER_SECOND = 60.0F;
constexpr float TIME_STEP = 1.0F / FRAMES_PER_SECOND;

auto main(int argc, char *argv[]) -> int {
    initializeLogging();
    spdlog::info("Starting {} application", PROJECT_NAME);

    // Default window size
    int windowWidth = WINDOW_WIDTH;
    int windowHeight = WINDOW_HEIGHT;
    bool fullscreen = DEFAULT_FULLSCREEN;
    bool developerMode = false;
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
            ("d,developerMode", "Developer mode", cxxopts::value<bool>(developerMode)->default_value("false"))
            ("help", "Print help");

        auto result = options.parse(argc, argv);

        if (result.count("help") != 0U) {
            std::cout << options.help() << std::endl;
            return 0;
        }
    }
    catch (const cxxopts::exceptions::exception &e) {
        spdlog::error("Error parsing options: {}", e.what());
        return 1;
    }

    // Load configuration file
    std::ifstream configFile("config.json");
    if (!configFile.is_open()) {
        spdlog::error("Failed to open config.json");
        return 1;
    }
    nlohmann::json config;
    configFile >> config;
    configFile.close();

    float maxWalkingSpeed = config["maxWalkingSpeed"];
    std::string preferredInputMethod = config["preferredInputMethod"];

    // Load character configuration file
    std::ifstream characterConfigFile("character_config.json");
    if (!characterConfigFile.is_open()) {
        spdlog::error("Failed to open character_config.json");
        return 1;
    }
    nlohmann::json characterConfig;
    characterConfigFile >> characterConfig;
    characterConfigFile.close();

    // Initialize SDL with video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        spdlog::error("SDL_Init Error: {}", SDL_GetError());
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
        spdlog::error("SDL_CreateWindow Error: {}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create SDL_Renderer with SDL_RENDERER_ACCELERATED and SDL_RENDERER_PRESENTVSYNC flags
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr) {
        spdlog::error("SDL_CreateRenderer Error: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    Box2DDebugDraw debugDraw(renderer, 1.0f);
    b2DebugDraw myDraw = b2DefaultDebugDraw();
    myDraw.context = &debugDraw;

    myDraw.drawShapes = true;
    myDraw.drawContacts = true;
    myDraw.drawJoints = true;
    myDraw.drawAABBs = true;

    myDraw.DrawPolygon = [](const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawPolygon(vertices, vertexCount, color, context);
    };
    myDraw.DrawSolidPolygon = [](b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawSolidPolygon(transform, vertices, vertexCount, radius, color, context);
    };
    myDraw.DrawCircle = [](b2Vec2 center, float radius, b2HexColor color, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawCircle(center, radius, color, context);
    };
    myDraw.DrawSolidCircle = [](b2Transform transform, float radius, b2HexColor color, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawSolidCircle(transform, radius, color, context);
    };
    myDraw.DrawSegment = [](b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawSegment(p1, p2, color, context);
    };
    myDraw.DrawTransform = [](b2Transform transform, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawTransform(transform, context);
    };
    myDraw.DrawPoint = [](b2Vec2 p, float size, b2HexColor color, void* context) {
        Box2DDebugDraw* renderer = static_cast<Box2DDebugDraw*>(context);
        renderer->DrawPoint(p, size, color, context);
    };

    // Get the display scale factor
    float displayScale = SDL_GetWindowDisplayScale(window);

    // Initialize Box2D World
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{GRAVITY_X, GRAVITY_Y};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // Create Level object
    constexpr uint32_t WORLD_HEIGHT = 24;
    Level level(renderer, worldId, assetDir, windowWidth, windowHeight, WORLD_HEIGHT);

    // Load tilemap
    std::string levelPath = assetDir + "/levels/" + levelName + ".tmj";
    if (!level.loadTilemap(levelPath)) {
        spdlog::error("Failed to load tilemap: {}", levelPath);
        level.handleErrors();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create Character object
    Character character(renderer, worldId, 15.0F, 20.0F, windowWidth, windowHeight, characterConfig);
    character.setMaxWalkingSpeed(maxWalkingSpeed);

    // Initialize ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Initialize DeveloperMenu with initial settings from characterConfig
    DeveloperMenu developerMenu(characterConfig);

    // Notify all observers of the initial settings if developer mode is enabled
    if (developerMode) {
        developerMenu.init(window, renderer);

        // Create and register GameSettingsObserver
        GameSettingsObserver gameSettingsObserver(worldId, character);
        developerMenu.addObserver(&gameSettingsObserver);

        developerMenu.notifyAllObservers();
    }

    bool running = true;
    bool showDebugWindow = false;
    constexpr int subStepCount = 8;

    while (running) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            // Handle other events (keyboard, mouse, etc.)
            character.handleInput(event);

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F2) {
                showDebugWindow = !showDebugWindow;
                character.showDebugWindow(showDebugWindow);
            }

            if (developerMode) {
                developerMenu.handleInput();

                if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F1) {
                    developerMenu.toggleVisibility();
                }
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Update physics
        b2World_Step(worldId, TIME_STEP, subStepCount);

        // Start the ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Update character
        character.checkGroundContact();
        character.update(TIME_STEP);

        // Game logic and rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, COLOR_ALPHA); // Clear with black color
        SDL_RenderClear(renderer);

        // Render level
        level.render();

        debugDraw.setScale(level.getScale());
        debugDraw.setOffset(level.getOffsetX(), level.getOffsetY());
        debugDraw.setWindowSize(windowWidth, windowHeight);

        if (developerMenu.isBox2DDebugDrawEnabled()) {
            myDraw.drawShapes = developerMenu.shouldDrawShapes();
            myDraw.drawJoints = developerMenu.shouldDrawJoints();
            myDraw.drawAABBs = developerMenu.shouldDrawAABBs();
            myDraw.drawContacts = developerMenu.shouldDrawContactPoints();
            myDraw.drawContactNormals = developerMenu.shouldDrawContactNormals();
            myDraw.drawContactImpulses = developerMenu.shouldDrawContactImpulses();
            myDraw.drawFrictionImpulses = developerMenu.shouldDrawFrictionImpulses();
            b2World_Draw(worldId, &myDraw);
        }

        // Render character
        character.render(level.getScale(), level.getOffsetX(), level.getOffsetY(), windowWidth, windowHeight);

        // Reset SDL renderer transformations before rendering ImGui
        SDL_SetRenderScale(renderer, displayScale, displayScale);
        SDL_SetRenderViewport(renderer, nullptr);

        // Render developer menu if in developer mode
        if (developerMode) {
            developerMenu.render();
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

        // Reset SDL renderer scale to 1.0 for next frame
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);

        SDL_RenderPresent(renderer);
    }
    spdlog::info("Exiting main game loop");

    // Save developer menu settings
    developerMenu.saveSettings();

    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    spdlog::info("Cleaned up resources and quit SDL");

    return 0;
}
