#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "Animation.h"
#include <unordered_map>
#include <string>
#include <deque>

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

    void setJumpStrength(float strength);
    void setJumpCooldownDuration(float duration);

    void handleJumpInput(bool keyDown);
    void applyJumpImpulse();
    void updateJumpState(float deltaTime);

    void setGroundAcceleration(float acceleration);
    void setAirAcceleration(float acceleration);

    void setShowDebugRectangles(bool value);
    void setShowContactPoints(bool value);
    void setShowForceVectors(bool value);
    void setMaxContactPoints(int maxPoints); 

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

    // Debug visualization member variables
    bool showDebugRectangles;
    bool showContactPoints;
    bool showForceVectors;
    SDL_Color debugColor;
    std::deque<b2Vec2> contactPoints; // Change to deque
    int maxContactPoints; // Add this line

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
    void updateDebugColor();
    SDL_Color interpolateColor(const SDL_Color& startColor, const SDL_Color& endColor, float t);
};
