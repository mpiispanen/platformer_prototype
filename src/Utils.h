#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>

// Conversion factor between physics units (meters) and screen units (pixels)
constexpr float PIXELS_PER_METER = 32.0F; // 32 pixels = 1 meter

// Convert Box2D world coordinates to SDL screen coordinates
auto Box2DToSDL(const b2Vec2& position, float extraScale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) -> SDL_FPoint;

// Convert SDL screen coordinates to Box2D world coordinates
auto SDLToBox2D(const SDL_FPoint& screenPos, float extraScale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight) -> b2Vec2;

#endif // UTILS_H