#ifndef DEVELOPER_MENU_H
#define DEVELOPER_MENU_H

#include <imgui.h>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

class DeveloperMenu {
public:
    DeveloperMenu();
    ~DeveloperMenu();

    void init(SDL_Window* window, SDL_Renderer* renderer);
    void render();
    void handleInput();
    void toggleVisibility();
    void loadSettings();
    void saveSettings();
    void handleDPIScaling(SDL_Window* window);

private:
    bool isVisible = false;
    float gravity;
    nlohmann::json settings;
};

#endif // DEVELOPER_MENU_H
