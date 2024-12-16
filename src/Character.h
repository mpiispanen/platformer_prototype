#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "Animation.h"
#include <unordered_map>
#include <string>

class Character {
public:
    Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y, uint32_t windowWidth, uint32_t windowHeight, const nlohmann::json& characterConfig);
    ~Character();

    void handleInput(const SDL_Event& event);
    void update(float deltaTime);
    void render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight);
    void setMaxWalkingSpeed(float speed);
    void showDebugWindow(bool show);
    void checkGroundContact();

private:
    SDL_Renderer* renderer;
    b2WorldId worldId;
    b2BodyId bodyId;
    SDL_Rect characterRectangle;
    Animation idleAnimation;
    Animation walkingAnimation;
    float maxWalkingSpeed;
    float acceleration;
    float deceleration;
    float x;
    float y;
    int windowWidth;
    int windowHeight;
    bool showDebug;
    bool isOnGround;

    bool isMoving {false};
    bool isMovingLeft {false};
    bool isMovingRight {false};
    bool isMovingUp {false};
    bool isMovingDown {false};
    bool isFacingRight {true};

    std::unordered_map<std::string, nlohmann::json> animationConfigs;

    float debugPrintInterval;
    float elapsedTime;

    void createBody();
    void loadIdleAnimation();
    void loadWalkingAnimation();
    void flipAnimation(bool faceRight);
    void updateDebugWindow();
};
