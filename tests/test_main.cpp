#include <gtest/gtest.h>
#include <cxxopts.hpp>
#include <SDL2/SDL.h>

// Test case for command-line argument parsing
TEST(CommandLineArgumentParsingTest, DefaultValues) {
    int argc = 1;
    const char* argv[] = {"./platformer_prototype"};
    cxxopts::Options options(argv[0], "Platformer Prototype");
    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>()->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>()->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>()->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, argv);

    EXPECT_EQ(result["width"].as<int>(), 800);
    EXPECT_EQ(result["height"].as<int>(), 600);
    EXPECT_EQ(result["fullscreen"].as<bool>(), false);
}

TEST(CommandLineArgumentParsingTest, CustomValues) {
    int argc = 4;
    const char* argv[] = {"./platformer_prototype", "--width", "1024", "--height", "768"};
    cxxopts::Options options(argv[0], "Platformer Prototype");
    options.add_options()
        ("w,width", "Window width", cxxopts::value<int>()->default_value("800"))
        ("h,height", "Window height", cxxopts::value<int>()->default_value("600"))
        ("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>()->default_value("false"))
        ("help", "Print help");

    auto result = options.parse(argc, argv);

    EXPECT_EQ(result["width"].as<int>(), 1024);
    EXPECT_EQ(result["height"].as<int>(), 768);
}

// Test case for SDL2 initialization
TEST(SDL2InitializationTest, InitializationSuccess) {
    EXPECT_EQ(SDL_Init(SDL_INIT_VIDEO), 0);
    SDL_Quit();
}

TEST(SDL2InitializationTest, InitializationFailure) {
    SDL_SetMainReady();
    EXPECT_NE(SDL_Init(SDL_INIT_VIDEO), 0);
    SDL_Quit();
}

// Test case for window creation and renderer setup
TEST(WindowCreationTest, WindowCreationSuccess) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Platformer Prototype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    EXPECT_NE(window, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

TEST(RendererSetupTest, RendererCreationSuccess) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Platformer Prototype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    EXPECT_NE(renderer, nullptr);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Test case for event handling
TEST(EventHandlingTest, QuitEvent) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Platformer Prototype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);

    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }

    EXPECT_FALSE(running);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Test case for game loop and resource cleanup
TEST(GameLoopTest, FrameRateControl) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Platformer Prototype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    while (running && frameCount < 60) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
        frameCount++;
    }
    Uint32 endTime = SDL_GetTicks();
    Uint32 elapsedTime = endTime - startTime;

    EXPECT_NEAR(elapsedTime, 1000, 50);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
