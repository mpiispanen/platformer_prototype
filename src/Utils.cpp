#include "Utils.h"

// Convert Box2D world coordinates to SDL rendering coordinates
SDL_FPoint Box2DToSDL(const b2Vec2& worldPos, float scale, float offsetX, float offsetY, int windowHeight) {
    SDL_FPoint screenPos;
    screenPos.x = worldPos.x * scale + offsetX;
    screenPos.y = windowHeight - (worldPos.y * scale + offsetY);
    return screenPos;
}

// Convert SDL rendering coordinates to Box2D world coordinates
b2Vec2 SDLToBox2D(const SDL_FPoint& screenPos, float scale, float offsetX, float offsetY, int windowHeight) {
    b2Vec2 worldPos;
    worldPos.x = (screenPos.x - offsetX) / scale;
    worldPos.y = (windowHeight - screenPos.y - offsetY) / scale;
    return worldPos;
}