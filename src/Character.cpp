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

Character::Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y, uint32_t windowWidth, uint32_t windowHeight, const nlohmann::json& characterConfig)
    : renderer(renderer), worldId(worldId), x(x), y(y), windowWidth(windowWidth), windowHeight(windowHeight), maxWalkingSpeed(5.0F), showDebug(false), isMoving(false), isOnGround(false) {
    spdlog::debug("Initializing character at position ({}, {})", x, y);

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
                isMovingLeft = keyDown;
                break;
            case SDLK_RIGHT:
            case SDLK_D:
                isMovingRight = keyDown;
                break;
            case SDLK_UP:
            case SDLK_W:
                isMovingUp = keyDown;
                break;
            case SDLK_DOWN:
            case SDLK_S:
                isMovingDown = keyDown;
                break;
            default:
                break;
        }
    }
}

void Character::update(float deltaTime) {
    elapsedTime += deltaTime;

    // Update character position and animation
    b2Vec2 position = b2Body_GetPosition(bodyId);
    x = position.x;
    y = position.y;

    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);

    if (isMovingLeft) {
        velocity.x = isOnGround ? -maxWalkingSpeed : -maxWalkingSpeed * 0.5F;
        isFacingRight = false;
    }
    if (isMovingRight) {
        velocity.x = isOnGround ? maxWalkingSpeed : maxWalkingSpeed * 0.5F;
        isFacingRight = true;
    }
    if (isMovingUp) {
        velocity.y = maxWalkingSpeed;
    }
    if (isMovingDown) {
        velocity.y = -maxWalkingSpeed;
    }

    b2Body_SetLinearVelocity(bodyId, velocity);

    if (velocity.x != 0 || velocity.y != 0) {
        walkingAnimation.update(deltaTime);
        isMoving = true;
    } else {
        idleAnimation.update(deltaTime);
        isMoving = false;
    }

    // Flip the animation based on the direction
    flipAnimation(isFacingRight);

    // Update the debug window
    if (showDebug) {
        updateDebugWindow();
    }
}

void Character::render(float extraScale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) {
    b2Vec2 position = b2Body_GetPosition(bodyId);
    SDL_FPoint screenPos = Box2DToSDL(position, extraScale, offsetX, offsetY, windowWidth, windowHeight);

    SDL_Texture* currentFrame = isMoving ? walkingAnimation.getCurrentFrame() : idleAnimation.getCurrentFrame();
    if (currentFrame != nullptr) {
        SDL_FRect dstRect = {
            screenPos.x - ((characterRectangle.w) / 2 * extraScale),
            screenPos.y - ((characterRectangle.h) / 2 * extraScale),
            characterRectangle.w * extraScale,
            characterRectangle.h * extraScale
        };
        SDL_RenderTextureRotated(renderer, currentFrame, nullptr, &dstRect, 0.0, nullptr, isMoving ? walkingAnimation.getFlip() : idleAnimation.getFlip());
    }
}

void Character::flipAnimation(bool faceRight) {
    SDL_FlipMode flip = faceRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    idleAnimation.setFlip(flip);
    walkingAnimation.setFlip(flip);
}

void Character::setMaxWalkingSpeed(float speed) {
    maxWalkingSpeed = speed;
}

void Character::createBody() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2{x, y};

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
