#include "Box2DDebugDraw.h"
#include "Utils.h"
#include <cstdint>

Box2DDebugDraw::Box2DDebugDraw(SDL_Renderer* renderer, float scale)
    : renderer(renderer), scale(scale) {}

void Box2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context) {
    RGBA8 rgba = MakeRGBA8(color, 1.0f);
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    for (int i = 0; i < vertexCount; ++i) {
        SDL_FPoint v1 = Box2DToSDL(vertices[i], scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_FPoint v2 = Box2DToSDL(vertices[(i + 1) % vertexCount], scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
    }
}

void Box2DDebugDraw::DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context) {
    RGBA8 rgba = MakeRGBA8(color, 1.0f);
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    for (int i = 0; i < vertexCount; ++i) {
        SDL_FPoint v1 = Box2DToSDL(vertices[i], scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_FPoint v2 = Box2DToSDL(vertices[(i + 1) % vertexCount], scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
    }
}

void Box2DDebugDraw::DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context) {
    RGBA8 rgba = MakeRGBA8(color, 1.0f);
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    const int segments = 16;
    const float increment = 2.0f * b2_pi / static_cast<float>(segments);
    float theta = 0.0f;
    for (int i = 0; i < segments; ++i) {
        b2Vec2 p1 = center + b2Vec2{radius * cosf(theta), radius * sinf(theta)};
        b2Vec2 p2 = center + b2Vec2{radius * cosf(theta + increment), radius * sinf(theta + increment)};
        SDL_FPoint v1 = Box2DToSDL(p1, scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_FPoint v2 = Box2DToSDL(p2, scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
        theta += increment;
    }
}

void Box2DDebugDraw::DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context) {
    RGBA8 rgba = MakeRGBA8(color, 1.0f);
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    const int segments = 16;
    const float increment = 2.0f * b2_pi / static_cast<float>(segments);
    float theta = 0.0f;
    for (int i = 0; i < segments; ++i) {
        b2Vec2 p1 = transform.p + b2Vec2{radius * cosf(theta), radius * sinf(theta)};
        b2Vec2 p2 = transform.p + b2Vec2{radius * cosf(theta + increment), radius * sinf(theta + increment)};
        SDL_FPoint v1 = Box2DToSDL(p1, scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_FPoint v2 = Box2DToSDL(p2, scale, offsetX, offsetY, windowWidth, windowHeight);
        SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
        theta += increment;
    }
    b2Vec2 p = transform.p + radius * b2Vec2{cosf(theta), sinf(theta)};
    SDL_FPoint v = Box2DToSDL(p, scale, 0, 0, 0, 0);
    SDL_RenderLine(renderer, transform.p.x * scale, transform.p.y * scale, v.x, v.y);
}

void Box2DDebugDraw::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context) {
    RGBA8 rgba = MakeRGBA8(color, 1.0f);
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    SDL_FPoint v1 = Box2DToSDL(p1, scale, offsetX, offsetY, windowWidth, windowHeight);
    SDL_FPoint v2 = Box2DToSDL(p2, scale, offsetX, offsetY, windowWidth, windowHeight);
    SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
}

void Box2DDebugDraw::DrawTransform(b2Transform transform, void* context) {
    const float k_axisScale = 0.4f;
    b2Vec2 p1 = transform.p, p2;

    // Draw x-axis
    p2 = p1 + k_axisScale * b2Rot_GetXAxis(transform.q);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FPoint v1 = Box2DToSDL(p1, scale, offsetX, offsetY, windowWidth, windowHeight);
    SDL_FPoint v2 = Box2DToSDL(p2, scale, offsetX, offsetY, windowWidth, windowHeight);
    SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);

    // Draw y-axis
    p2 = p1 + k_axisScale * b2Rot_GetYAxis(transform.q);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    v2 = Box2DToSDL(p2, scale, offsetX, offsetY, windowWidth, windowHeight);
    SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
}

void Box2DDebugDraw::DrawPoint(b2Vec2 p, float size, b2HexColor color, void* context) {
    RGBA8 rgba = MakeRGBA8(color, 1.0f);
    SDL_SetRenderDrawColor(renderer, rgba.r, rgba.g, rgba.b, rgba.a);
    SDL_FPoint v = Box2DToSDL(p, scale, offsetX, offsetY, windowWidth, windowHeight);
    SDL_FRect rect;
    rect.x = v.x - size / 2;
    rect.y = v.y - size / 2;
    rect.w = size;
    rect.h = size;
    SDL_RenderFillRect(renderer, &rect);
}

void Box2DDebugDraw::DrawString(b2Vec2 p, const char* s, void* context) {
    // Implement text rendering if needed, for now, we can leave it as a placeholder
}