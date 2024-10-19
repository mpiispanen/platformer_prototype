#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Default window size
    int windowWidth = 800;
    int windowHeight = 600;
    bool fullscreen = false;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "w:h:f")) != -1)
    {
        switch (opt)
        {
        case 'w':
            windowWidth = std::stoi(optarg);
            break;
        case 'h':
            windowHeight = std::stoi(optarg);
            break;
        case 'f':
            fullscreen = true;
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [-w width] [-h height] [-f fullscreen]" << std::endl;
            return 1;
        }
    }

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create the application window
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (fullscreen)
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }
    SDL_Window *window = SDL_CreateWindow("Platformer Prototype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);
    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main game loop
    bool running = true;
    while (running)
    {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                SDL_SetWindowSize(window, windowWidth, windowHeight);
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                    // Handle other key events here
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                // Handle mouse button events here
            }
        }

        // Update game logic here

        // Render the game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Draw game objects here
        SDL_RenderPresent(renderer);

        // Control frame rate (target 60 FPS)
        SDL_Delay(16);
    }

    // Clean up resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
