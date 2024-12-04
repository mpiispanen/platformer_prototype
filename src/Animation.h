#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL3/SDL.h>
#include <vector>

class Animation {
public:
    Animation();
    ~Animation();

    void addFrame(SDL_Texture* texture, int duration);
    void update(float deltaTime);
    [[nodiscard]] auto getCurrentFrame() const -> SDL_Texture*;
    [[nodiscard]] auto getFlip() const -> SDL_FlipMode;
    void setFlip(SDL_FlipMode flip);

private:
    struct Frame {
        SDL_Texture* texture;
        int duration;
    };

    std::vector<Frame> frames;
    int currentFrameIndex;
    float currentTime;
    SDL_FlipMode flip;
};

#endif // ANIMATION_H
