#include "Tile.h"
#include "Utils.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

Tile::Tile(SDL_Renderer* renderer, const std::string& type, b2BodyId bodyId, b2ShapeId shapeId, uint width, uint height, const std::string& assetDir)
    : renderer(renderer), bodyId(bodyId), shapeId(shapeId), width(width), height(height), type(type) {
    std::string texturePath = assetDir + "/tiles/" + type + ".png";
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load texture: " << texturePath << " Error: " << SDL_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
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

void Tile::render(float scale, float offsetX, float offsetY, int windowHeight) {
    SDL_FPoint screenPos = Box2DToSDL(worldPos, scale, offsetX, offsetY, windowHeight);
    
    SDL_FRect dstRect;
    dstRect.x = static_cast<int>(screenPos.x);
    dstRect.y = static_cast<int>(screenPos.y);
    dstRect.w = static_cast<int>(getWidth() * scale);
    dstRect.h = static_cast<int>(getHeight() * scale);
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
}

void Tile::updateAnimation(float deltaTime) {
    animation.update(deltaTime);
}

int Tile::getX() const {
    return worldPos.x;
}

int Tile::getY() const {
    return worldPos.y;
}

uint Tile::getWidth() const {
    return width;
}

uint Tile::getHeight() const {
    return height;
}

const std::string& Tile::getType() const {
    return type;
}
