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

    // Methods for setting jump-related parameters
    void setJumpStrength(float strength);
    void setJumpCooldownDuration(float duration);

    // Methods for handling jump input, applying jump force, and updating jump state
    void handleJumpInput(bool keyDown);
    void applyJumpImpulse();
    void updateJumpState(float deltaTime);

    // Methods for setting acceleration values
    void setGroundAcceleration(float acceleration);
    void setAirAcceleration(float acceleration);

private:
    SDL_Renderer* renderer;
    b2WorldId worldId;
    b2BodyId bodyId;
    SDL_Rect characterRectangle;
    Animation idleAnimation;
    Animation walkingAnimation;
    Animation jumpingAnimation;
    Animation fallingAnimation;
    Animation landingAnimation;
    Animation* currentAnimation; 
    
    float maxWalkingSpeed;
    float groundAcceleration;
    float airAcceleration;
    float deceleration;
    b2Vec2 position;
    int windowWidth;
    int windowHeight;
    bool showDebug;
    bool isOnGround;
    bool wasOnGround;

    bool moveLeftRequested {false};
    bool moveRightRequested {false};
    bool jumpRequested {false};
    bool isFacingRight {true};

    std::unordered_map<std::string, nlohmann::json> animationConfigs;

    float debugPrintInterval;
    float elapsedTime;
    float timeSinceLastGroundContact;

    // Jump-related member variables
    float jumpStrength;
    float minJumpHeight;
    float maxJumpHeight;
    float jumpCooldownDuration;
    float jumpInputDuration;
    float jumpCooldownTimer;

    void createBody();
    void loadIdleAnimation();
    void loadWalkingAnimation();
    void loadJumpingAnimation();
    void loadFallingAnimation();
    void loadLandingAnimation();
    void flipAnimation(bool faceRight);
    void updateDebugWindow();
    void displayCurrentAnimationInfo();
    void applyMovement(float deltaTime);
};
