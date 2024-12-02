#include "Character.h"
#include "Utils.h"
#include <SDL3_image/SDL_image.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

// Scale factor for character size (4x)
constexpr float CHARACTER_SCALE = 4.0F;
constexpr float TILE_SIZE = 32.0F;

Character::Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y, uint32_t windowWidth, uint32_t windowHeight, const nlohmann::json& animationConfig)
    : renderer(renderer), worldId(worldId), x(x), y(y), windowWidth(windowWidth), windowHeight(windowHeight), maxWalkingSpeed(5.0f), isMoving(false) {
    // Store the provided configuration data
    for (const auto& animation : animationConfig["animations"]) {
        std::string name = animation["name"];
        animationConfigs[name] = animation;
    }
    
    createBody();
    loadIdleAnimation();
    loadWalkingAnimation();
}

Character::~Character() {
    b2DestroyBody(bodyId);
}

void Character::handleInput(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool keyDown = (event.type == SDL_EVENT_KEY_DOWN);

        switch (event.key.key) {
            case SDLK_LEFT:
            case SDLK_A:
                movingLeft = keyDown;
                break;
            case SDLK_RIGHT:
            case SDLK_D:
                movingRight = keyDown;
                break;
            case SDLK_UP:
            case SDLK_W:
                movingUp = keyDown;
                break;
            case SDLK_DOWN:
            case SDLK_S:
                movingDown = keyDown;
                break;
            default:
                break;
        }
    }
}

void Character::update(float deltaTime) {
    // Update character position and animation
    b2Vec2 position = b2Body_GetPosition(bodyId);
    x = position.x;
    y = position.y;

    b2Vec2 velocity = b2Vec2_zero;

    if (movingLeft) {
        velocity.x -= maxWalkingSpeed;
        facingRight = false;
    }
    if (movingRight) {
        velocity.x += maxWalkingSpeed;
        facingRight = true;
    }
    if (movingUp) {
        velocity.y += maxWalkingSpeed;
    }
    if (movingDown) {
        velocity.y -= maxWalkingSpeed;
    }

    b2Body_SetLinearVelocity(bodyId, velocity);

    if (velocity.x != 0 || velocity.y != 0) {
        walkingAnimation.update(deltaTime);
        moving = true;
    } else {
        idleAnimation.update(deltaTime);
        moving = false;
    }

    flipAnimation(facingRight);
}

void Character::render(float extraScale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) {
    b2Vec2 position = b2Body_GetPosition(bodyId);
    SDL_FPoint screenPos = Box2DToSDL(position, extraScale, offsetX, offsetY, windowWidth, windowHeight);

    SDL_Texture* currentFrame = moving ? walkingAnimation.getCurrentFrame() : idleAnimation.getCurrentFrame();
    if (currentFrame != nullptr) {
        SDL_FRect dstRect = {
            screenPos.x - ((TILE_SIZE * CHARACTER_SCALE) / 2 * extraScale),
            screenPos.y - ((TILE_SIZE * CHARACTER_SCALE) / 2 * extraScale),
            TILE_SIZE * CHARACTER_SCALE * extraScale,
            TILE_SIZE * CHARACTER_SCALE * extraScale
        };
        SDL_RenderTextureRotated(renderer, currentFrame, nullptr, &dstRect, 0.0, nullptr, moving ? walkingAnimation.getFlip() : idleAnimation.getFlip());
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
    float halfWidth = (TILE_SIZE * CHARACTER_SCALE) / (2.0F * PIXELS_PER_METER);
    float halfHeight = (TILE_SIZE * CHARACTER_SCALE) / (2.0F * PIXELS_PER_METER);
    b2Polygon box = b2MakeBox(halfWidth, halfHeight);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 5.0F;
    shapeDef.friction = 1.0F;
    b2CreatePolygonShape(bodyId, &shapeDef, &box);
}

void Character::loadIdleAnimation() {
    auto& config = animationConfigs["idle"];
    SDL_Surface* surface = IMG_Load(config["filePath"].get<std::string>().c_str());
    if (surface == nullptr) {
        std::cerr << "Failed to load idle animation: " << SDL_GetError() << std::endl;
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int frameCount = config["frameCount"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (int i = 0; i < frameCount; ++i) {
        SDL_Surface* frameSurface = SDL_CreateSurface(frameWidth, frameHeight, SDL_PIXELFORMAT_RGBA8888);
        SDL_Rect srcRect = {i * frameWidth, 0, frameWidth, frameHeight};
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
        std::cerr << "Failed to load walking animation: " << SDL_GetError() << std::endl;
        return;
    }

    int frameWidth = config["frameSize"]["width"];
    int frameHeight = config["frameSize"]["height"];
    int frameCount = config["frameCount"];
    int animationSpeed = static_cast<int>(config["animationSpeed"].get<float>() * 1000);

    for (int i = 0; i < frameCount; ++i) {
        SDL_Surface* frameSurface = SDL_CreateSurface(frameWidth, frameHeight, SDL_PIXELFORMAT_RGBA8888);
        SDL_Rect srcRect = {i * frameWidth, 0, frameWidth, frameHeight};
        SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

        SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        walkingAnimation.addFrame(frameTexture, animationSpeed);

        SDL_DestroySurface(frameSurface);
    }

    SDL_DestroySurface(surface);
}
