#pragma once
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "Observer.h"

class DeveloperMenu {
public:
    DeveloperMenu(const nlohmann::json& initialSettings);
    ~DeveloperMenu();

    void init(SDL_Window* window, SDL_Renderer* renderer);
    void handleDPIScaling(SDL_Window* window);
    void render();
    void handleInput();
    void loadSettings();
    void saveSettings();
    void toggleVisibility();
    void addObserver(Observer* observer);
    void notifyObservers(const std::string& settingName, float newValue);
    void notifyAllObservers();

private:
    bool isVisible;
    float gravity;
    float characterSpeed;

    float prevGravity;
    float prevCharacterSpeed;

    std::vector<Observer*> observers;
    nlohmann::json settings;

    float jumpStrength;
    float minJumpHeight;
    float maxJumpHeight;
    float jumpCooldownDuration;

    float prevJumpStrength;
    float prevMinJumpHeight;
    float prevMaxJumpHeight;
    float prevJumpCooldownDuration;

    float groundAcceleration;
    float airAcceleration;
    float maxWalkingSpeed;

    float prevGroundAcceleration;
    float prevAirAcceleration;
    float prevMaxWalkingSpeed;
};
