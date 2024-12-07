#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include "Tile.h"

class Level {
public:
    Level(SDL_Renderer* renderer, b2WorldId worldId, std::string& assetDir, int windowWidth, int windowHeight, int tilesVertically);
    ~Level();

    auto loadTilemap(const std::string& filename) -> bool;
    void render();
    void handleErrors();
    void update(float deltaTime, const b2Vec2& characterPosition);

    void setScale(float newScale);
    void setViewportCenter(float centerX, float centerY);

    [[nodiscard]] auto getScale() const -> float;
    [[nodiscard]] auto getOffsetX() const -> float;
    [[nodiscard]] auto getOffsetY() const -> float;

private:
    void createTile(const std::string& type, int x, int y, bool isDynamic);

    SDL_Renderer* renderer;
    b2WorldId worldId;
    std::string assetDir;

    int windowWidth;
    int windowHeight;
    int tilesVertically;
    float scale;
    float offsetX;
    float offsetY;

    uint32_t tileWidth;
    uint32_t tileHeight;
    
    std::vector<std::shared_ptr<Tile>> tiles;
};
