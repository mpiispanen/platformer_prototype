#include "Character.h"
#include "Utils.h"
#include <SDL3_image/SDL_image.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <imgui.h>

// Define equality operator for b2ShapeId
inline bool operator==(const b2ShapeId& lhs, const b2ShapeId& rhs) {
    return lhs.index1 == rhs.index1;
}

// Scale factor for character size (4x)
constexpr float TILE_SIZE = 32.0F;
constexpr float LANDING_THRESHOLD = 0.2F; // Threshold time for landing animation

Character::Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y, uint32_t windowWidth, uint32_t windowHeight, const nlohmann::json& characterConfig)
    : renderer(renderer), worldId(worldId), windowWidth(windowWidth), windowHeight(windowHeight), showDebug(false), isOnGround(false), jumpCooldownTimer(0.0F), elapsedTime(0.0F), timeSinceLastGroundContact(0.0F) {
    position = {x, y};
    
    spdlog::debug("Initializing character at position ({}, {})", position.x, position.y);

    // Store the provided configuration data
    for (const auto& animation : characterConfig["animations"]) {
        std::string name = animation["name"];
        animationConfigs[name] = animation;
    }

    // Read character size from config
    characterRectangle = {.x=characterConfig["initialPosition"]["x"], .y=characterConfig["initialPosition"]["y"], .w=characterConfig["characterSize"]["width"], .h=characterConfig["characterSize"]["height"]};

    createBody();
    loadIdleAnimation();
    loadWalkingAnimation();
    loadJumpingAnimation();
    loadFallingAnimation();
    loadLandingAnimation();

    currentAnimation = &idleAnimation;

    // Initialize acceleration values
    groundAcceleration = characterConfig["groundAcceleration"];
    airAcceleration = characterConfig["airAcceleration"];
    maxWalkingSpeed = characterConfig["maxWalkingSpeed"];
    jumpStrength = characterConfig["jumpStrength"];
}

Character::~Character() {
    spdlog::debug("Destroying character");
    b2DestroyBody(bodyId);
}

void Character::handleInput(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool keyDown = (event.type == SDL_EVENT_KEY_DOWN);
        switch (event.key.key) {
            case SDLK_LEFT:
            case SDLK_A:
                moveLeftRequested = keyDown;
                break;
            case SDLK_RIGHT:
            case SDLK_D:
                moveRightRequested = keyDown;
                break;
            case SDLK_UP:
            case SDLK_W:
                handleJumpInput(keyDown); // Call handleJumpInput here
                break;
        }
    }
}

void Character::update(float deltaTime) {
    elapsedTime += deltaTime;

    applyMovement(deltaTime);

    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);

    checkGroundContact();

    position = b2Body_GetPosition(bodyId);

    Animation* newAnimation = nullptr;

    if (isOnGround) {
        if (!wasOnGround && timeSinceLastGroundContact > LANDING_THRESHOLD) {
            newAnimation = &landingAnimation;
        } else if (std::abs(velocity.x) > 0.1f) {
            newAnimation = &walkingAnimation;
        } else {
            newAnimation = &idleAnimation;
        }
        timeSinceLastGroundContact = 0.0f; // Reset time since last ground contact when on the ground
    } else {
        if (velocity.y >= 0) {
            newAnimation = &jumpingAnimation;
        } else if (velocity.y < 0) {
            newAnimation = &fallingAnimation;
        }
        timeSinceLastGroundContact += deltaTime; // Increment time since last ground contact when in the air
    }

    if (newAnimation && currentAnimation != newAnimation) {
        if (currentAnimation != &landingAnimation || currentAnimation->hasCompleted()) {
            currentAnimation = newAnimation;
            currentAnimation->reset(); // Reset animation to frame 0
        }
    }

    currentAnimation->update(deltaTime);

    if (showDebug) {
        updateDebugWindow();
    }

    wasOnGround = isOnGround;
}

void Character::render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) {
    // Fetch the current character position from the physics body
    position = b2Body_GetPosition(bodyId);

    // Convert the position to screen coordinates
    SDL_FPoint screenPos = Box2DToSDL(position, scale, offsetX, offsetY, windowWidth, windowHeight);

    // Render character using current animation
    SDL_Texture* currentFrame = currentAnimation->getCurrentFrame();
    if (currentFrame) {
        SDL_FRect dstRect = {
            screenPos.x - ((characterRectangle.w) / 2 * scale),
            screenPos.y - ((characterRectangle.h) / 2 * scale),
            characterRectangle.w * scale,
            characterRectangle.h * scale

        };
        SDL_RenderTextureRotated(renderer, currentFrame, nullptr, &dstRect, 0.0, nullptr, currentAnimation->getFlip());
    }
}

void Character::flipAnimation(bool faceRight) {
    SDL_FlipMode flip = faceRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    idleAnimation.setFlip(flip);
    walkingAnimation.setFlip(flip);
    jumpingAnimation.setFlip(flip);
    fallingAnimation.setFlip(flip);
    landingAnimation.setFlip(flip);
}

void Character::setMaxWalkingSpeed(float speed) {
    maxWalkingSpeed = speed;
}

void Character::setGroundAcceleration(float acceleration) {
    groundAcceleration = acceleration;
}

void Character::setAirAcceleration(float acceleration) {
    airAcceleration = acceleration;
}

void Character::setJumpStrength(float strength) {
    jumpStrength = strength;
}

// TODO: Should be used to disable jumping too often
void Character::setJumpCooldownDuration(float duration) {
    jumpCooldownDuration = duration;
}

void Character::handleJumpInput(bool keyDown) {
    jumpRequested = keyDown;
    if (keyDown && isOnGround) {
        applyJumpImpulse();
    }
}

void Character::applyJumpImpulse() {
    b2Vec2 impulse = {0.0F, jumpStrength};
    b2Body_ApplyLinearImpulse(bodyId, impulse, position, true);
    isOnGround = false;
    jumpRequested = false; // Reset jumpRequested after applying jump impulse
}

void Character::applyMovement(float deltaTime) {
    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
    float acceleration = isOnGround ? groundAcceleration : airAcceleration;

    // Handle horizontal movement
    if (moveLeftRequested) {
        velocity.x = std::max(velocity.x - acceleration * deltaTime, -maxWalkingSpeed);
        if (isFacingRight) {
            isFacingRight = false;
            flipAnimation(isFacingRight);
        }
    }
    if (moveRightRequested) {
        velocity.x = std::min(velocity.x + acceleration * deltaTime, maxWalkingSpeed);
        if (!isFacingRight) {
            isFacingRight = true;
            flipAnimation(isFacingRight);
        }
    }

    b2Body_SetLinearVelocity(bodyId, velocity);
}

void Character::createBody() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;

    bodyId = b2CreateBody(worldId, &bodyDef);

    // Scale physics body to match visual size (in meters)
    float halfWidth = (characterRectangle.w) / (2.0F * PIXELS_PER_METER);
    float halfHeight = (characterRectangle.h) / (2.0F * PIXELS_PER_METER);
    b2Polygon box = b2MakeBox(halfWidth, halfHeight);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0F; // Set realistic density
    shapeDef.friction = 0.3F; // Set realistic friction
    shapeDef.enableContactEvents = true; // Enable contact events
    b2CreatePolygonShape(bodyId, &shapeDef, &box);

    // Ensure gravity scale is set to 1.0
    b2Body_SetGravityScale(bodyId, 1.0F);
}

void Character::loadIdleAnimation() {
    auto& config = animationConfigs["idle"];
    SDL_Surface* surface = IMG_Load(config["filePath"].get<std::string>().c_str());
    if (surface == nullptr) {
        spdlog::error("Failed to load idle animation: {}", SDL_GetError());
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int characterSpriteWidth = config["characterSpriteSize"]["width"];
    int characterSpriteHeight = config["characterSpriteSize"]["height"];
    int characterSpritePosX = config["characterSpritePosition"]["x"];
    int characterSpritePosY = config["characterSpritePosition"]["y"];
    int frameCount = config["frameCount"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (int i = 0; i < frameCount; ++i) {
        SDL_Surface* frameSurface = SDL_CreateSurface(characterSpriteWidth, characterSpriteHeight, SDL_PIXELFORMAT_RGBA8888);
        SDL_Rect srcRect = {
            characterSpritePosX + (i * frameWidth) - (characterSpriteWidth/2),
            characterSpritePosY + (characterSpriteHeight/2),
            characterSpriteWidth,
            characterSpriteHeight
        };
        SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

        SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        idleAnimation.addFrame(frameTexture, animationSpeed);

        SDL_DestroySurface(frameSurface);
    }

    SDL_DestroySurface(surface);
}

void Character::loadWalkingAnimation() {
    auto& config = animationConfigs["walking"];
    SDL_Surface* surface = IMG_Load(config["filePath"].get<std::string>().c_str());
    if (surface == nullptr) {
        spdlog::error("Failed to load walking animation: {}", SDL_GetError());
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int characterSpriteWidth = config["characterSpriteSize"]["width"];
    int characterSpriteHeight = config["characterSpriteSize"]["height"];
    int characterSpritePosX = config["characterSpritePosition"]["x"];
    int characterSpritePosY = config["characterSpritePosition"]["y"];
    int frameCount = config["frameCount"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (int i = 0; i < frameCount; ++i) {
        SDL_Surface* frameSurface = SDL_CreateSurface(characterSpriteWidth, characterSpriteHeight, SDL_PIXELFORMAT_RGBA8888);
        SDL_Rect srcRect = {
            characterSpritePosX + (i * frameWidth) - (characterSpriteWidth/2),
            characterSpritePosY + (characterSpriteHeight/2),
            characterSpriteWidth,
            characterSpriteHeight
        };
        SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

        SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        walkingAnimation.addFrame(frameTexture, animationSpeed);

        SDL_DestroySurface(frameSurface);
    }

    SDL_DestroySurface(surface);
}

void Character::loadJumpingAnimation() {
    auto& config = animationConfigs["jumping"];
    SDL_Surface* surface = IMG_Load(config["filePath"].get<std::string>().c_str());
    if (surface == nullptr) {
        spdlog::error("Failed to load jumping animation: {}", SDL_GetError());
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int characterSpriteWidth = config["characterSpriteSize"]["width"];
    int characterSpriteHeight = config["characterSpriteSize"]["height"];
    int characterSpritePosX = config["characterSpritePosition"]["x"];
    int characterSpritePosY = config["characterSpritePosition"]["y"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (const auto& frame : config["frames"]) {
        if (frame["type"] == "jump") {
            int startFrame = frame["startFrame"];
            int frameCount = frame["frameCount"];
            bool looping = frame.value("looping", config.value("looping", false));
            for (int i = 0; i < frameCount; ++i) {
                SDL_Surface* frameSurface = SDL_CreateSurface(characterSpriteWidth, characterSpriteHeight, SDL_PIXELFORMAT_RGBA8888);
                SDL_Rect srcRect = {
                    characterSpritePosX + ((startFrame + i) * frameWidth) - (characterSpriteWidth / 2),
                    characterSpritePosY + (characterSpriteHeight / 2),
                    characterSpriteWidth,
                    characterSpriteHeight
                };
                SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

                SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
                jumpingAnimation.addFrame(frameTexture, animationSpeed);

                SDL_DestroySurface(frameSurface);
            }
            jumpingAnimation.setLooping(looping);
        }
    }

    SDL_DestroySurface(surface);
}

void Character::loadFallingAnimation() {
    auto& config = animationConfigs["jumping"];
    SDL_Surface* surface = IMG_Load(config["filePath"].get<std::string>().c_str());
    if (surface == nullptr) {
        spdlog::error("Failed to load falling animation: {}", SDL_GetError());
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int characterSpriteWidth = config["characterSpriteSize"]["width"];
    int characterSpriteHeight = config["characterSpriteSize"]["height"];
    int characterSpritePosX = config["characterSpritePosition"]["x"];
    int characterSpritePosY = config["characterSpritePosition"]["y"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (const auto& frame : config["frames"]) {
        if (frame["type"] == "falling") {
            int startFrame = frame["startFrame"];
            int frameCount = frame["frameCount"];
            bool looping = frame.value("looping", config.value("looping", false));
            for (int i = 0; i < frameCount; ++i) {
                SDL_Surface* frameSurface = SDL_CreateSurface(characterSpriteWidth, characterSpriteHeight, SDL_PIXELFORMAT_RGBA8888);
                SDL_Rect srcRect = {
                    characterSpritePosX + ((startFrame + i) * frameWidth) - (characterSpriteWidth / 2),
                    characterSpritePosY + (characterSpriteHeight / 2),
                    characterSpriteWidth,
                    characterSpriteHeight
                };
                SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

                SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
                fallingAnimation.addFrame(frameTexture, animationSpeed);

                SDL_DestroySurface(frameSurface);
            }
            fallingAnimation.setLooping(looping);
        }
    }

    SDL_DestroySurface(surface);
}

void Character::loadLandingAnimation() {
    auto& config = animationConfigs["jumping"];
    SDL_Surface* surface = IMG_Load(config["filePath"].get<std::string>().c_str());
    if (surface == nullptr) {
        spdlog::error("Failed to load landing animation: {}", SDL_GetError());
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int characterSpriteWidth = config["characterSpriteSize"]["width"];
    int characterSpriteHeight = config["characterSpriteSize"]["height"];
    int characterSpritePosX = config["characterSpritePosition"]["x"];
    int characterSpritePosY = config["characterSpritePosition"]["y"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (const auto& frame : config["frames"]) {
        if (frame["type"] == "landing") {
            int startFrame = frame["startFrame"];
            int frameCount = frame["frameCount"];
            bool looping = frame.value("looping", config.value("looping", false));
            for (int i = 0; i < frameCount; ++i) {
                SDL_Surface* frameSurface = SDL_CreateSurface(characterSpriteWidth, characterSpriteHeight, SDL_PIXELFORMAT_RGBA8888);
                SDL_Rect srcRect = {
                    characterSpritePosX + ((startFrame + i) * frameWidth) - (characterSpriteWidth / 2),
                    characterSpritePosY + (characterSpriteHeight / 2),
                    characterSpriteWidth,
                    characterSpriteHeight
                };
                SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

                SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
                landingAnimation.addFrame(frameTexture, animationSpeed);

                SDL_DestroySurface(frameSurface);
            }
            landingAnimation.setLooping(looping);
        }
    }

    SDL_DestroySurface(surface);
}

void Character::showDebugWindow(bool show) {
    showDebug = show;
}

void Character::updateDebugWindow() {
    if (!showDebug) return;

    ImGui::Begin("Character Debug Info", &showDebug);
    
    // Position info
    b2Vec2 position = b2Body_GetPosition(bodyId);
    ImGui::Text("Position: (%.2f, %.2f)", position.x, position.y);
    
    // Velocity info
    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
    ImGui::Text("Velocity: (%.2f, %.2f)", velocity.x, velocity.y);
    float speed = b2Length(velocity);
    ImGui::Text("Speed: %.2f", speed);
    
    // Ground contact status
    ImGui::Text("On Ground: %s", isOnGround ? "Yes" : "No");
    
    ImGui::End();

    // Display current animation info
    displayCurrentAnimationInfo();
}

void Character::displayCurrentAnimationInfo() {
    std::string currentAnimationName;
    int currentFrameIndex = 0;
    int totalFrames = 0;

    if (currentAnimation == &walkingAnimation) {
        currentAnimationName = "walking";
        currentFrameIndex = walkingAnimation.getCurrentFrameIndex();
        totalFrames = walkingAnimation.getTotalFrames();
    } else if (currentAnimation == &jumpingAnimation) {
        currentAnimationName = "jumping";
        currentFrameIndex = jumpingAnimation.getCurrentFrameIndex();
        totalFrames = jumpingAnimation.getTotalFrames();
    } else if (currentAnimation == &fallingAnimation) {
        currentAnimationName = "falling";
        currentFrameIndex = fallingAnimation.getCurrentFrameIndex();
        totalFrames = fallingAnimation.getTotalFrames();
    } else if (currentAnimation == &landingAnimation) {
        currentAnimationName = "landing";
        currentFrameIndex = landingAnimation.getCurrentFrameIndex();
        totalFrames = landingAnimation.getTotalFrames();
    } else {
        currentAnimationName = "idle";
        currentFrameIndex = idleAnimation.getCurrentFrameIndex();
        totalFrames = idleAnimation.getTotalFrames();
    }

    ImGui::Begin("Character Info");
    ImGui::Text("Current Animation: %s", currentAnimationName.c_str());
    ImGui::Text("Frame: %d/%d", currentFrameIndex + 1, totalFrames);
    ImGui::End();
}

void Character::checkGroundContact() {
    b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);

    // Check begin contact events
    for (int i = 0; i < contactEvents.beginCount; ++i) {
        b2ContactBeginTouchEvent beginEvent = contactEvents.beginEvents[i];
        b2ShapeId shapeArray[1];
        b2Body_GetShapes(bodyId, shapeArray, 1);
        if (beginEvent.shapeIdA == shapeArray[0] || beginEvent.shapeIdB == shapeArray[0]) {
            b2ContactData contactData[10];
            int count = b2Body_GetContactData(bodyId, contactData, 10);
            for (int j = 0; j < count; ++j) {
                if (contactData[j].manifold.normal.y < 0) {
                    isOnGround = true;
                    break;
                }
            }
        }
    }

    // Check end contact events
    for (int i = 0; i < contactEvents.endCount; ++i) {
        b2ContactEndTouchEvent endEvent = contactEvents.endEvents[i];
        b2ShapeId shapeArray[1];
        b2Body_GetShapes(bodyId, shapeArray, 1);
        if (endEvent.shapeIdA == shapeArray[0] || endEvent.shapeIdB == shapeArray[0]) {
            b2ContactData contactData[10];
            int count = b2Body_GetContactData(bodyId, contactData, 10);
            bool groundContactFound = false;
            for (int j = 0; j < count; ++j) {
                if (contactData[j].manifold.normal.y < 0) {
                    groundContactFound = true;
                    break;
                }
            }
            if (!groundContactFound) {
                isOnGround = false;
            }
        }
    }
}
