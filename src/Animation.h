#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <SDL3/SDL.h>

class Animation {
public:
    Animation();
    ~Animation();

    void addFrame(SDL_Texture* texture, uint32_t duration_ms);
    void update(float deltaTime);
    [[nodiscard]] auto getCurrentFrame() const -> SDL_Texture*;

private:
    struct Frame {
        SDL_Texture* texture;
        uint32_t duration; // Duration in milliseconds
    };

    std::vector<Frame> frames;
    int currentFrameIndex;
    float elapsedTime;
};

#endif // ANIMATION_H
