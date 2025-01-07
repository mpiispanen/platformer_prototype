#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <string>
#include "Animation.h"

class Tile {
public:
    static const int TILE_SIZE = 32; // Assuming each tile is 32x32 pixels

    Tile(SDL_Renderer* renderer, const std::string& type, b2BodyId bodyId, b2ChainId chainId, b2ShapeId shapeId, uint32_t width, uint32_t height, const std::string& assetDir, int x, int y);
    ~Tile();

    void update();
    void render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight);
    void renderPolygonOutline(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight);
    void updateAnimation(float deltaTime);

    [[nodiscard]] auto getX() const -> int;
    [[nodiscard]] auto getY() const -> int;
    [[nodiscard]] auto getWidth() const -> uint32_t;
    [[nodiscard]] auto getHeight() const -> uint32_t;
    [[nodiscard]] auto getType() const -> const std::string&;

private:
    std::string type;
    int x;
    int y;
    uint32_t width;
    uint32_t height;
    b2BodyId bodyId;
    b2ChainId chainId;
    b2ShapeId shapeId;

    SDL_Renderer* renderer;
    SDL_Texture* texture;
    Animation animation;

    // Debug visualization member variables
    bool showForceVectors;
};

// Comparison operators for b2ChainId
bool operator==(const b2ChainId& lhs, const b2ChainId& rhs);
bool operator!=(const b2ChainId& lhs, const b2ChainId& rhs);
