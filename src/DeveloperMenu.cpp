#include "DeveloperMenu.h"
#include <fstream>
#include <SDL3/SDL_video.h>
#include <iostream>
#include "Logging.h"

DeveloperMenu::DeveloperMenu(const nlohmann::json& initialSettings)
    : isVisible(false),
      gravity(9.8f),
      characterSpeed(5.0f),
      prevGravity(9.8f),
      prevCharacterSpeed(5.0f),
      jumpStrength(initialSettings.value("jumpStrength", 10.0f)),
      minJumpHeight(initialSettings.value("minJumpHeight", 1.0f)),
      maxJumpHeight(initialSettings.value("maxJumpHeight", 5.0f)),
      jumpCooldownDuration(initialSettings.value("jumpCooldownDuration", 0.5f)),
      prevJumpStrength(jumpStrength),
      prevMinJumpHeight(minJumpHeight),
      prevMaxJumpHeight(maxJumpHeight),
      prevJumpCooldownDuration(jumpCooldownDuration),
      groundAcceleration(initialSettings.value("groundAcceleration", 10.0f)),
      airAcceleration(initialSettings.value("airAcceleration", 5.0f)),
      maxWalkingSpeed(initialSettings.value("maxWalkingSpeed", 7.0f)),
      prevGroundAcceleration(groundAcceleration),
      prevAirAcceleration(airAcceleration),
      prevMaxWalkingSpeed(maxWalkingSpeed),
      showDebugVisualizations(false),
      showContactPoints(false),
      showForceVisualizations(false),
      maxContactPoints(initialSettings.value("maxContactPoints", 10)) { // Add this line
    loadSettings();
}

DeveloperMenu::~DeveloperMenu() {
    saveSettings();
}

void DeveloperMenu::init(SDL_Window* window, SDL_Renderer* renderer) {
    // Handle DPI scaling
    handleDPIScaling(window);
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
    if (ImGui::SliderFloat("Gravity", &gravity, 0.0f, -30.0f)) {
        notifyObservers("gravity", gravity);
    }
    ImGui::Text("Character Speed: %.2f", characterSpeed);
    if (ImGui::SliderFloat("Character Speed", &characterSpeed, 0.0f, 20.0f)) {
        notifyObservers("characterSpeed", characterSpeed);
    }
    ImGui::Text("Jump Strength: %.2f", jumpStrength);
    if (ImGui::SliderFloat("Jump Strength", &jumpStrength, 0.0f, 50.0f)) {
        notifyObservers("jumpStrength", jumpStrength);
    }
    ImGui::Text("Minimum Jump Height: %.2f", minJumpHeight);
    if (ImGui::SliderFloat("Minimum Jump Height", &minJumpHeight, 0.0f, 10.0f)) {
        notifyObservers("minJumpHeight", minJumpHeight);
    }
    ImGui::Text("Maximum Jump Height: %.2f", maxJumpHeight);
    if (ImGui::SliderFloat("Maximum Jump Height", &maxJumpHeight, 0.0f, 20.0f)) {
        notifyObservers("maxJumpHeight", maxJumpHeight);
    }
    ImGui::Text("Jump Cooldown Duration: %.2f", jumpCooldownDuration);
    if (ImGui::SliderFloat("Jump Cooldown Duration", &jumpCooldownDuration, 0.0f, 5.0f)) {
        notifyObservers("jumpCooldownDuration", jumpCooldownDuration);
    }
    ImGui::Text("Ground Acceleration: %.2f", groundAcceleration);
    if (ImGui::SliderFloat("Ground Acceleration", &groundAcceleration, 0.0f, 20.0f)) {
        notifyObservers("groundAcceleration", groundAcceleration);
    }
    ImGui::Text("Air Acceleration: %.2f", airAcceleration);
    if (ImGui::SliderFloat("Air Acceleration", &airAcceleration, 0.0f, 20.0f)) {
        notifyObservers("airAcceleration", airAcceleration);
    }
    ImGui::Text("Max Walking Speed: %.2f", maxWalkingSpeed);
    if (ImGui::SliderFloat("Max Walking Speed", &maxWalkingSpeed, 0.0f, 20.0f)) {
        notifyObservers("maxWalkingSpeed", maxWalkingSpeed);
    }
    if (ImGui::Checkbox("Show Debug Visualizations", &showDebugVisualizations)) {
        notifyObservers("showDebugVisualizations", showDebugVisualizations);
    }
    if (ImGui::Checkbox("Show Contact Points", &showContactPoints)) {
        notifyObservers("showContactPoints", showContactPoints);
    }
    if (ImGui::Checkbox("Show Force Visualizations", &showForceVisualizations)) {
        notifyObservers("showForceVisualizations", showForceVisualizations);
    }
    ImGui::Text("Max Contact Points: %d", maxContactPoints);
    if (ImGui::SliderInt("Max Contact Points", &maxContactPoints, 1, 100)) {
        notifyObservers("maxContactPoints", static_cast<float>(maxContactPoints));
    }
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
        characterSpeed = settings.value("characterSpeed", 5.0f);
        jumpStrength = settings.value("jumpStrength", 10.0f);
        jumpCooldownDuration = settings.value("jumpCooldownDuration", 0.5f);
        groundAcceleration = settings.value("groundAcceleration", 10.0f);
        airAcceleration = settings.value("airAcceleration", 5.0f);
        maxWalkingSpeed = settings.value("maxWalkingSpeed", 7.0f);
        showDebugVisualizations = settings.value("showDebugVisualizations", false);
        showContactPoints = settings.value("showContactPoints", false);
        showForceVisualizations = settings.value("showForceVisualizations", false);
        maxContactPoints = settings.value("maxContactPoints", 10); 
        prevGravity = gravity;
        prevCharacterSpeed = characterSpeed;
        prevJumpStrength = jumpStrength;
        prevMinJumpHeight = minJumpHeight;
        prevMaxJumpHeight = maxJumpHeight;
        prevJumpCooldownDuration = jumpCooldownDuration;
        prevGroundAcceleration = groundAcceleration;
        prevAirAcceleration = airAcceleration;
        prevMaxWalkingSpeed = maxWalkingSpeed;
    } else {
        spdlog::warn("Failed to open developer_menu_settings.json. Using default settings.");
    }
}

void DeveloperMenu::saveSettings() {
    settings["gravity"] = gravity;
    settings["characterSpeed"] = characterSpeed;
    settings["jumpStrength"] = jumpStrength;
    settings["jumpCooldownDuration"] = jumpCooldownDuration;
    settings["groundAcceleration"] = groundAcceleration;
    settings["airAcceleration"] = airAcceleration;
    settings["maxWalkingSpeed"] = maxWalkingSpeed;
    settings["showDebugVisualizations"] = showDebugVisualizations;
    settings["showContactPoints"] = showContactPoints;
    settings["showForceVisualizations"] = showForceVisualizations;
    settings["maxContactPoints"] = maxContactPoints;
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

void DeveloperMenu::addObserver(Observer* observer) {
    observers.push_back(observer);
}

void DeveloperMenu::notifyObservers(const std::string& settingName, float newValue) {
    for (Observer* observer : observers) {
        observer->onSettingChanged(settingName, newValue);
    }
}

void DeveloperMenu::notifyAllObservers() {
    notifyObservers("gravity", gravity);
    notifyObservers("characterSpeed", characterSpeed);
    notifyObservers("jumpStrength", jumpStrength);
    notifyObservers("jumpCooldownDuration", jumpCooldownDuration);
    notifyObservers("groundAcceleration", groundAcceleration);
    notifyObservers("airAcceleration", airAcceleration);
    notifyObservers("maxWalkingSpeed", maxWalkingSpeed);
    notifyObservers("showDebugVisualizations", showDebugVisualizations);
    notifyObservers("showContactPoints", showContactPoints);
    notifyObservers("showForceVisualizations", showForceVisualizations);
    notifyObservers("maxContactPoints", static_cast<float>(maxContactPoints)); // Add this line
}
