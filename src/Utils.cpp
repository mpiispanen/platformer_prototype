#include "Utils.h"

// Convert Box2D world coordinates to SDL rendering coordinates
SDL_FPoint Box2DToSDL(const b2Vec2& worldPos, int windowHeight) {
    SDL_FPoint screenPos;
    screenPos.x = worldPos.x * PIXELS_PER_METER;
    screenPos.y = windowHeight - (worldPos.y * PIXELS_PER_METER);
    return screenPos;
}

// Convert SDL rendering coordinates to Box2D world coordinates
b2Vec2 SDLToBox2D(const SDL_FPoint& screenPos, int windowHeight) {
    b2Vec2 worldPos;
    worldPos.x = screenPos.x / PIXELS_PER_METER;
    worldPos.y = (windowHeight - screenPos.y) / PIXELS_PER_METER;
    return worldPos;
}