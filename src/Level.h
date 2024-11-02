#ifndef LEVEL_H
#define LEVEL_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <vector>
#include <memory>
#include <string>
#include "Tile.h"

class Level {
public:
    Level(SDL_Renderer* renderer, b2WorldId worldId, std::string& assetDir, int windowWidth, int windowHeight, int tilesVertically);
    ~Level();

    bool loadTilemap(const std::string& filename);
    void render();
    void handleErrors();

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

    uint tileWidth;
    uint tileHeight;
    
    std::vector<std::shared_ptr<Tile>> tiles;
};

#endif // LEVEL_H
