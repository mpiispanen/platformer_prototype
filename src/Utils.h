#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>

// Convert Box2D world coordinates to SDL rendering coordinates
SDL_FPoint Box2DToSDL(b2Vec2& worldPos, float scale, float offsetX, float offsetY, int windowHeight);

// Convert SDL rendering coordinates to Box2D world coordinates
b2Vec2 SDLToBox2D(SDL_FPoint& screenPos, float scale, float offsetX, float offsetY, int windowHeight);

#endif // UTILS_H