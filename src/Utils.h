#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>

// Conversion factor between physics units (meters) and screen units (pixels)
constexpr float PIXELS_PER_METER = 32.0f; // 32 pixels = 1 meter

// Convert Box2D world coordinates to SDL screen coordinates
SDL_FPoint Box2DToSDL(const b2Vec2& position, int windowHeight);

// Convert SDL screen coordinates to Box2D world coordinates
b2Vec2 SDLToBox2D(const SDL_FPoint& screenPos, int windowHeight);

#endif // UTILS_H