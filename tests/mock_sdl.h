#ifndef MOCK_SDL_H
#define MOCK_SDL_H

#include <gmock/gmock.h>
#include <SDL2/SDL.h>

class MockSDL {
public:
    MOCK_METHOD(int, SDL_Init, (Uint32 flags), ());
    MOCK_METHOD(void, SDL_Quit, (), ());
    MOCK_METHOD(SDL_Window*, SDL_CreateWindow, (const char* title, int x, int y, int w, int h, Uint32 flags), ());
    MOCK_METHOD(void, SDL_DestroyWindow, (SDL_Window* window), ());
    MOCK_METHOD(SDL_Renderer*, SDL_CreateRenderer, (SDL_Window* window, int index, Uint32 flags), ());
    MOCK_METHOD(void, SDL_DestroyRenderer, (SDL_Renderer* renderer), ());
    MOCK_METHOD(int, SDL_PollEvent, (SDL_Event* event), ());
    MOCK_METHOD(void, SDL_Delay, (Uint32 ms), ());
    MOCK_METHOD(const char*, SDL_GetError, (), ());
};

MockSDL* mock_sdl = nullptr;

#define SDL_Init(flags) mock_sdl->SDL_Init(flags)
#define SDL_Quit() mock_sdl->SDL_Quit()
#define SDL_CreateWindow(title, x, y, w, h, flags) mock_sdl->SDL_CreateWindow(title, x, y, w, h, flags)
#define SDL_DestroyWindow(window) mock_sdl->SDL_DestroyWindow(window)
#define SDL_CreateRenderer(window, index, flags) mock_sdl->SDL_CreateRenderer(window, index, flags)
#define SDL_DestroyRenderer(renderer) mock_sdl->SDL_DestroyRenderer(renderer)
#define SDL_PollEvent(event) mock_sdl->SDL_PollEvent(event)
#define SDL_Delay(ms) mock_sdl->SDL_Delay(ms)
#define SDL_GetError() mock_sdl->SDL_GetError()

#endif // MOCK_SDL_H
