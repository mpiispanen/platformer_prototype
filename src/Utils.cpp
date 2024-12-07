#include "Utils.h"
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

// Convert Box2D world coordinates to SDL rendering coordinates
auto Box2DToSDL(const b2Vec2& worldPos, float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) -> SDL_FPoint {
    float total_scale = PIXELS_PER_METER * scale;
    SDL_FPoint screenPos;
    screenPos.x = ((worldPos.x - offsetX) * total_scale) + (windowWidth / 2.0f);
    screenPos.y = windowHeight - ((worldPos.y - offsetY - (windowHeight / total_scale / 2)) * total_scale);
    return screenPos;
}

// Convert SDL rendering coordinates to Box2D world coordinates
auto SDLToBox2D(const SDL_FPoint& screenPos, float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) -> b2Vec2 {
    float total_scale = PIXELS_PER_METER * scale;
    b2Vec2 worldPos;
    worldPos.x = (screenPos.x - (windowWidth / 2.0f)) / total_scale + offsetX;
    worldPos.y = ((windowHeight - screenPos.y) / total_scale) + offsetY + (windowHeight / total_scale / 2);
    return worldPos;
}