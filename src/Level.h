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
    std::tuple<int, int, int> findStartingTile(const std::vector<std::pair<int, int>>& chainTiles, const std::vector<std::vector<bool>>& visited);
    std::pair<std::vector<std::pair<int, int>>, std::vector<b2Vec2>> traceBorder(int startX, int startY, int startDir, const std::vector<std::pair<int, int>>& chainTiles, std::vector<std::vector<bool>>& visited, int mapHeight, int tileWidth, int tileHeight);
    void createChainForStaticTiles(const std::vector<std::pair<int, int>> &chainTiles, int mapHeight);
    void update(float deltaTime, const b2Vec2& characterPosition);

    void setScale(float newScale);
    void setViewportCenter(float centerX, float centerY);

    [[nodiscard]] auto getScale() const -> float;
    [[nodiscard]] auto getOffsetX() const -> float;
    [[nodiscard]] auto getOffsetY() const -> float;

    void setShowPolygonOutlines(bool show);

private:
    void createTile(const std::string& type, int x, int y, bool isDynamic);
    bool isSolidTile(int x, int y, const std::vector<std::pair<int, int>> &chainTiles);
    void initializeDebugDraw();

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
    bool showPolygonOutlines;
};
