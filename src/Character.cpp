#include "Character.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_surface.h>
#include <iostream>

Character::Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y)
    : renderer(renderer), worldId(worldId), x(x), y(y) {
    createBody();
    loadIdleAnimation();
}

Character::~Character() {
    b2DestroyBody(bodyId);
}

void Character::handleInput(const SDL_Event& event) {
    // Handle user input here
}

void Character::update(float deltaTime) {
    // Update character position and animation
    b2Vec2 position = b2Body_GetPosition(bodyId);
    x = position.x;
    y = position.y;

    std::cout << "Character position: (" << x << ", " << y << ")" << std::endl;

    idleAnimation.update(deltaTime);
}

void Character::render() {
    SDL_FPoint screenPos = {x, y};
    SDL_Texture* currentFrame = idleAnimation.getCurrentFrame();
    if (currentFrame != nullptr) {
        SDL_FRect dstRect = {screenPos.x, screenPos.y, 32, 32};
        SDL_RenderTexture(renderer, currentFrame, nullptr, &dstRect);
    }
}

void Character::createBody() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2{x, y};

    bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon box = b2MakeBox(16.0F, 16.0F);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0F;
    shapeDef.friction = 0.3F;
    b2CreatePolygonShape(bodyId, &shapeDef, &box);
}

void Character::loadIdleAnimation() {
    SDL_Surface* surface = IMG_Load("assets/characters/cat/idle.png");
    if (surface == nullptr) {
        std::cerr << "Failed to load idle animation: " << SDL_GetError() << std::endl;
        return;
    }

    int frameWidth = 32;
    int frameHeight = 32;
    int frameCount = surface->w / frameWidth;

    for (int i = 0; i < frameCount; ++i) {
        SDL_Surface* frameSurface = SDL_CreateSurface(frameWidth, frameHeight, SDL_PIXELFORMAT_RGBA8888);
        SDL_Rect srcRect = {i * frameWidth, 0, frameWidth, frameHeight};
        SDL_BlitSurface(surface, &srcRect, frameSurface, nullptr);

        SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        idleAnimation.addFrame(frameTexture, 100);

        SDL_DestroySurface(frameSurface);
    }

    SDL_DestroySurface(surface);
}
