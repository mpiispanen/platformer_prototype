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
      maxContactPoints(initialSettings.value("maxContactPoints", 10)),
      enableBox2DDebugDraw(initialSettings.value("enableBox2DDebugDraw", false)),
      drawShapes(initialSettings.value("drawShapes", true)),
      drawJoints(initialSettings.value("drawJoints", true)),
      drawAABBs(initialSettings.value("drawAABBs", false)),
      drawContactPoints(initialSettings.value("drawContactPoints", false)),
      drawContactNormals(initialSettings.value("drawContactNormals", false)),
      drawContactImpulses(initialSettings.value("drawContactImpulses", false)),
      drawFrictionImpulses(initialSettings.value("drawFrictionImpulses", false)),
      ambientLightIntensity(initialSettings.value("ambientLightIntensity", 0.5f)),
      ambientLightColor(1.0f, 1.0f, 1.0f, 1.0f) {
    if (initialSettings.contains("ambientLightColor")) {
        const auto& color = initialSettings["ambientLightColor"];
        ambientLightColor.x = color.value("r", 1.0f);
        ambientLightColor.y = color.value("g", 1.0f);
        ambientLightColor.z = color.value("b", 1.0f);
    }

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

    // Box2D Debug Draw Options
    if (ImGui::Checkbox("Enable Box2D Debug Draw", &enableBox2DDebugDraw)) {
        notifyObservers("enableBox2DDebugDraw", enableBox2DDebugDraw);
    }
    if (enableBox2DDebugDraw) {
        if (ImGui::Checkbox("Draw Shapes", &drawShapes)) {
            notifyObservers("drawShapes", drawShapes);
        }
        if (ImGui::Checkbox("Draw Joints", &drawJoints)) {
            notifyObservers("drawJoints", drawJoints);
        }
        if (ImGui::Checkbox("Draw AABBs", &drawAABBs)) {
            notifyObservers("drawAABBs", drawAABBs);
        }
        if (ImGui::Checkbox("Draw Contact Points", &drawContactPoints)) {
            notifyObservers("drawContactPoints", drawContactPoints);
        }
        if (ImGui::Checkbox("Draw Contact Normals", &drawContactNormals)) {
            notifyObservers("drawContactNormals", drawContactNormals);
        }
        if (ImGui::Checkbox("Draw Contact Impulses", &drawContactImpulses)) {
            notifyObservers("drawContactImpulses", drawContactImpulses);
        }
        if (ImGui::Checkbox("Draw Friction Impulses", &drawFrictionImpulses)) {
            notifyObservers("drawFrictionImpulses", drawFrictionImpulses);
        }
    }

    // Ambient Light Settings
    ImGui::Text("Ambient Light Intensity: %.2f", ambientLightIntensity);
    if (ImGui::SliderFloat("Ambient Light Intensity", &ambientLightIntensity, 0.0f, 1.0f)) {
        notifyObservers("ambientLightIntensity", ambientLightIntensity);
    }
    
    if (ImGui::ColorEdit3("Ambient Light Color", (float*)&ambientLightColor)) {
        notifyObservers("ambientLightColor", ambientLightColor.x);
        notifyObservers("ambientLightColor", ambientLightColor.y);
        notifyObservers("ambientLightColor", ambientLightColor.z);
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
        enableBox2DDebugDraw = settings.value("enableBox2DDebugDraw", false);
        drawShapes = settings.value("drawShapes", true);
        drawJoints = settings.value("drawJoints", true);
        drawAABBs = settings.value("drawAABBs", false);
        drawContactPoints = settings.value("drawContactPoints", false);
        drawContactNormals = settings.value("drawContactNormals", false);
        drawContactImpulses = settings.value("drawContactImpulses", false);
        drawFrictionImpulses = settings.value("drawFrictionImpulses", false);
        ambientLightIntensity = settings.value("ambientLightIntensity", 0.5f);
        ambientLightColor = ImVec4(settings["ambientLightColor"]["r"], settings["ambientLightColor"]["g"], settings["ambientLightColor"]["b"], 1.0f);
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
    settings["enableBox2DDebugDraw"] = enableBox2DDebugDraw;
    settings["drawShapes"] = drawShapes;
    settings["drawJoints"] = drawJoints;
    settings["drawAABBs"] = drawAABBs;
    settings["drawContactPoints"] = drawContactPoints;
    settings["drawContactNormals"] = drawContactNormals;
    settings["drawContactImpulses"] = drawContactImpulses;
    settings["drawFrictionImpulses"] = drawFrictionImpulses;
    settings["ambientLightIntensity"] = ambientLightIntensity;
    settings["ambientLightColor"]["r"] = ambientLightColor.x;
    settings["ambientLightColor"]["g"] = ambientLightColor.y;
    settings["ambientLightColor"]["b"] = ambientLightColor.z;
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
    notifyObservers("maxContactPoints", static_cast<float>(maxContactPoints));
    notifyObservers("enableBox2DDebugDraw", enableBox2DDebugDraw);
    notifyObservers("drawShapes", drawShapes);
    notifyObservers("drawJoints", drawJoints);
    notifyObservers("drawAABBs", drawAABBs);
    notifyObservers("drawContactPoints", drawContactPoints);
    notifyObservers("drawContactNormals", drawContactNormals);
    notifyObservers("drawContactImpulses", drawContactImpulses);
    notifyObservers("drawFrictionImpulses", drawFrictionImpulses);
    notifyObservers("ambientLightIntensity", ambientLightIntensity);
    notifyObservers("ambientLightColor", ambientLightColor.x);
    notifyObservers("ambientLightColor", ambientLightColor.y);
    notifyObservers("ambientLightColor", ambientLightColor.z);
}
