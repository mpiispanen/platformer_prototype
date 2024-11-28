#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include "Animation.h"

class Character {
public:
    Character(SDL_Renderer* renderer, b2WorldId worldId, float x, float y, uint32_t windowWidth, uint32_t windowHeight);
    ~Character();

    void handleInput(const SDL_Event& event);
    void update(float deltaTime);
    void render(float scale, float offsetX, float offsetY, uint32_t windowWidth, uint32_t windowHeight);

private:
    SDL_Renderer* renderer;
    b2WorldId worldId;
    b2BodyId bodyId;
    Animation idleAnimation;
    float x;
    float y;
    uint32_t windowWidth;
    uint32_t windowHeight;

    void createBody();
    void loadIdleAnimation();
};

#endif // CHARACTER_H
