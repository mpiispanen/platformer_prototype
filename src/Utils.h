#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>

// Convert Box2D world coordinates to SDL screen coordinates
auto Box2DToSDL(b2Vec2& worldPos, float scale, float offsetX, float offsetY, int windowHeight) -> SDL_FPoint;

// Convert SDL screen coordinates to Box2D world coordinates
auto SDLToBox2D(SDL_FPoint& screenPos, float scale, float offsetX, float offsetY, int windowHeight) -> b2Vec2;

#endif // UTILS_H