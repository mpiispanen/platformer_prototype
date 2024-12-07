#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <nlohmann/json.hpp>
#include "Animation.h"
#include <unordered_map>
#include <string>

class Character {
public:
    Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y, uint32_t windowWidth, uint32_t windowHeight, const nlohmann::json& animationConfig);
    ~Character();

    void handleInput(const SDL_Event& event);
    void update(float deltaTime);
    void render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight);
    void setMaxWalkingSpeed(float speed);

private:
    SDL_Renderer* renderer;
    b2WorldId worldId;
    b2BodyId bodyId;
    Animation idleAnimation;
    Animation walkingAnimation;
    
    bool isMoving;
    SDL_FRect characterRectangle;
    float maxWalkingSpeed;
    float acceleration;
    float deceleration;
    float x;
    float y;
    uint32_t windowWidth;
    uint32_t windowHeight;

    bool moving {false};
    bool movingLeft {false};
    bool movingRight {false};
    bool movingUp {false};
    bool movingDown {false};
    bool facingRight {true};

    std::unordered_map<std::string, nlohmann::json> animationConfigs;

    void createBody();
    void loadIdleAnimation();
    void loadWalkingAnimation();
    void flipAnimation(bool faceRight);
};
