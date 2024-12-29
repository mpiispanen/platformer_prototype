#include "Tile.h"
#include "Utils.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <spdlog/spdlog.h>

Tile::Tile(SDL_Renderer* renderer, const std::string& type, b2BodyId bodyId, b2ShapeId shapeId, uint32_t width, uint32_t height, const std::string& assetDir)
    : renderer(renderer), bodyId(bodyId), shapeId(shapeId), width(width), height(height), type(type), showForceVectors(false) {
    std::string texturePath = assetDir + "/tiles/" + type + ".png";
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (surface == nullptr) {
        std::cerr << "Failed to load texture: " << texturePath << " Error: " << SDL_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (texture == nullptr) {
        std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
    }
}

Tile::~Tile() {
    b2DestroyShape(shapeId, true);
    shapeId = b2_nullShapeId;

    b2DestroyBody(bodyId);
    bodyId = b2_nullBodyId;

    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

void Tile::update() {
    b2Transform transform = b2Body_GetTransform(bodyId);
    worldPos = transform.p;
}

void Tile::render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) {
    b2Vec2 position = b2Body_GetPosition(bodyId);
    SDL_FPoint screenPos = Box2DToSDL(position, scale, offsetX, offsetY, windowWidth, windowHeight);

    SDL_FRect dstRect;
    dstRect.x = static_cast<int>(screenPos.x - (width * scale / 2));
    dstRect.y = static_cast<int>(screenPos.y - (height * scale / 2));
    dstRect.w = static_cast<int>(width * scale);
    dstRect.h = static_cast<int>(height * scale);

    //spdlog::info("Drawing tile of type {} at logical position ({}, {}) and screen position ({}, {})", 
    //             type, position.x, position.y, screenPos.x, screenPos.y);

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

    if (showForceVectors) {
        b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId); // Assuming you want to get the linear velocity instead of force
        SDL_FPoint forceEndPos = {
            screenPos.x + (velocity.x * scale),
            screenPos.y - (velocity.y * scale)
        };
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for gravity
        SDL_RenderLine(renderer, screenPos.x, screenPos.y, forceEndPos.x, forceEndPos.y);
    }
}

void Tile::updateAnimation(float deltaTime) {
    animation.update(deltaTime);
}

auto Tile::getX() const -> int {
    return worldPos.x;
}

auto Tile::getY() const -> int {
    return worldPos.y;
}

auto Tile::getWidth() const -> uint32_t {
    return width;
}

auto Tile::getHeight() const -> uint32_t {
    return height;
}

auto Tile::getType() const -> const std::string& {
    return type;
}
