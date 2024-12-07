#include "DeveloperMenu.h"
#include <fstream>
#include <SDL3/SDL_video.h>
#include <iostream>
#include "Logging.h"

DeveloperMenu::DeveloperMenu() : isVisible(false), gravity(9.8f) {
    loadSettings();
}

DeveloperMenu::~DeveloperMenu() {
    saveSettings();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void DeveloperMenu::init(SDL_Window* window, SDL_Renderer* renderer) {
    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    // Handle DPI scaling
    handleDPIScaling(window);

    // Initialize ImGui for SDL and SDL_Renderer
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void DeveloperMenu::handleDPIScaling(SDL_Window* window) {
    // Get the display scale factor
    float scale = SDL_GetWindowDisplayScale(window);
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale; // Assuming 96 DPI is the baseline
    io.DisplayFramebufferScale = ImVec2(scale, scale);
}

void DeveloperMenu::render() {
    if (!isVisible) return;

    // Set default window size
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("Developer Menu");
    ImGui::Text("Gravity: %.2f", gravity);
    ImGui::SliderFloat("Gravity", &gravity, 0.0f, 20.0f);
    ImGui::End();
}

void DeveloperMenu::handleInput() {
    // Handle input events for the developer menu
}

void DeveloperMenu::loadSettings() {
    std::ifstream settingsFile("developer_menu_settings.json");
    if (settingsFile.is_open()) {
        settingsFile >> settings;
        settingsFile.close();
        gravity = settings.value("gravity", 9.8f);
    } else {
        spdlog::warn("Failed to open developer_menu_settings.json. Using default settings.");
    }
}

void DeveloperMenu::saveSettings() {
    settings["gravity"] = gravity;
    std::ofstream settingsFile("developer_menu_settings.json");
    if (settingsFile.is_open()) {
        settingsFile << settings.dump(4);
        settingsFile.close();
    } else {
        spdlog::error("Failed to save developer_menu_settings.json.");
    }
}

void DeveloperMenu::toggleVisibility() {
    isVisible = !isVisible;
}
