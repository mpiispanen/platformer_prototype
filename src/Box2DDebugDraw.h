#pragma once

#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Utils.h"

struct RGBA8
{
	uint8_t r, g, b, a;
};

static inline RGBA8 MakeRGBA8( b2HexColor c, float alpha )
{
	return { uint8_t( ( c >> 16 ) & 0xFF ), uint8_t( ( c >> 8 ) & 0xFF ), uint8_t( c & 0xFF ), uint8_t( 0xFF * alpha ) };
}

class Box2DDebugDraw {
public:
    Box2DDebugDraw(SDL_Renderer* renderer, float scale);

    void setScale(float newScale) { scale = newScale; }
    void setOffset(float newOffsetX, float newOffsetY) { offsetX = newOffsetX; offsetY = newOffsetY; }
    void setWindowSize(uint32_t newWindowWidth, uint32_t newWindowHeight) { windowWidth = newWindowWidth; windowHeight = newWindowHeight; }

    void DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);
    void DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color,
								void* context);
    void DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context);
    void DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context);
    void DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context);
    void DrawTransform(b2Transform transform, void* context);
    void DrawPoint(b2Vec2 p, float size, b2HexColor color, void* context);
    void DrawString(b2Vec2 p, const char* s, void* context);

private:
    SDL_Renderer* renderer;
    float scale;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    uint32_t windowWidth = 0;
    uint32_t windowHeight = 0;
};