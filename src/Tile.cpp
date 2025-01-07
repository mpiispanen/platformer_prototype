#include "Tile.h"
#include "Utils.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <spdlog/spdlog.h>

Tile::Tile(SDL_Renderer* renderer, const std::string& type, b2BodyId bodyId, b2ChainId chainId, b2ShapeId shapeId, uint32_t width, uint32_t height, const std::string& assetDir, int x, int y)
    : renderer(renderer), bodyId(bodyId), chainId(chainId), shapeId(shapeId), width(width), height(height), type(type), x(x), y(y), showForceVectors(false) {
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
    // Chain shapes are managed by the Level class, so we don't destroy them here
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

void Tile::update() {
}

void Tile::render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) {
    b2Vec2 position = {static_cast<float>(x), static_cast<float>(y)};
    SDL_FPoint screenPos = Box2DToSDL(position, scale, offsetX, offsetY, windowWidth, windowHeight);

    SDL_FRect dstRect;
    dstRect.x = static_cast<int>(screenPos.x);
    dstRect.y = static_cast<int>(screenPos.y - (height * scale));
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

void Tile::renderPolygonOutline(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) {
    if (chainId == b2_nullChainId) return;

    b2Polygon polygon = b2Shape_GetPolygon(shapeId);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for polygon outlines

    for (int i = 0; i < polygon.count; ++i) {
        b2Vec2 p1 = polygon.vertices[i];
        b2Vec2 p2 = polygon.vertices[(i + 1) % polygon.count];

        SDL_FPoint screenP1 = Box2DToSDL(p1, scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_FPoint screenP2 = Box2DToSDL(p2, scale, offsetX, offsetY, windowWidth, windowHeight);

        SDL_RenderLine(renderer, screenP1.x, screenP1.y, screenP2.x, screenP2.y);
    }
}

void Tile::updateAnimation(float deltaTime) {
    animation.update(deltaTime);
}

auto Tile::getX() const -> int {
    return x;
}

auto Tile::getY() const -> int {
    return y;
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

// Comparison operators for b2ChainId
bool operator==(const b2ChainId& lhs, const b2ChainId& rhs) {
    return lhs.index1 == rhs.index1 && lhs.world0 == rhs.world0 && lhs.revision == rhs.revision;
}

bool operator!=(const b2ChainId& lhs, const b2ChainId& rhs) {
    return !(lhs == rhs);
}
