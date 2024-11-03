#ifndef TILE_H
#define TILE_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <string>
#include "Animation.h"

class Tile {
public:
    static const int TILE_SIZE = 32; // Assuming each tile is 32x32 pixels

    Tile(SDL_Renderer* renderer, const std::string& type, b2BodyId bodyId, b2ShapeId shapeId, uint32_t width, uint32_t height, const std::string& assetDir);
    ~Tile();

    void update();
    void render(float scale, float offsetX, float offsetY, int windowHeight);
    void updateAnimation(float deltaTime);

    int getX() const;
    int getY() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    const std::string& getType() const;

private:
    std::string type;
    b2Vec2 worldPos;
    uint32_t width;
    uint32_t height;
    b2BodyId bodyId;
    b2ShapeId shapeId;

    SDL_Renderer* renderer;
    SDL_Texture* texture;
    Animation animation;
};

#endif // TILE_H
