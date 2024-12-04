#include "Utils.h"

// Convert Box2D world coordinates to SDL rendering coordinates
auto Box2DToSDL(const b2Vec2& worldPos, float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) -> SDL_FPoint {
    float total_scale = PIXELS_PER_METER * scale;
    constexpr float halfMeter = 0.5F;
    SDL_FPoint screenPos;
    screenPos.x = (worldPos.x - offsetX + (windowWidth / total_scale / 2) - halfMeter) * total_scale;
    screenPos.y = windowHeight - ((worldPos.y - offsetY - (windowHeight / total_scale / 2) - halfMeter) * total_scale);
    return screenPos;
}

// Convert SDL rendering coordinates to Box2D world coordinates
auto SDLToBox2D(const SDL_FPoint& screenPos, float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) -> b2Vec2 {
    float total_scale = PIXELS_PER_METER * scale;
    b2Vec2 worldPos;

    constexpr float halfMeter = 0.5F;
    worldPos.x = (screenPos.x / total_scale) + offsetX - (windowWidth / total_scale / 2) + halfMeter;
    worldPos.y = ((windowHeight - screenPos.y) / total_scale) + offsetY - (windowHeight / total_scale / 2) + halfMeter;
    return worldPos;
}