#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <string>

class Animation {
public:
    Animation();
    ~Animation();

    void addFrame(SDL_Texture* texture, int duration);
    void update(float deltaTime);
    [[nodiscard]] auto getCurrentFrame() const -> SDL_Texture*;
    [[nodiscard]] auto getFlip() const -> SDL_FlipMode;
    void setFlip(SDL_FlipMode flip);
    void setLooping(bool looping);
    void reset();
    void setName(const std::string& name);
    [[nodiscard]] auto getName() const -> std::string;
    [[nodiscard]] auto getCurrentFrameIndex() const -> int;
    [[nodiscard]] auto getTotalFrames() const -> int;

private:
    struct Frame {
        SDL_Texture* texture;
        int duration;
    };

    std::vector<Frame> frames;
    int currentFrameIndex;
    float currentTime;
    SDL_FlipMode flip;
    bool isLooping;
    std::string name;
};
