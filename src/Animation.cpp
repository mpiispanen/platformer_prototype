#include "Animation.h"

Animation::Animation()
    : currentFrameIndex(0), elapsedTime(0.0F) {}

Animation::~Animation() {
    for (auto& frame : frames) {
        SDL_DestroyTexture(frame.texture);
    }
}

void Animation::addFrame(SDL_Texture* texture, uint32_t duration_ms) {
    frames.push_back({texture, duration_ms});
}

void Animation::update(float deltaTime) {
    if (frames.empty()) {
        return;
    }

    constexpr uint32_t MS_PER_SECOND = 1000;
    elapsedTime += deltaTime * MS_PER_SECOND;
    if (elapsedTime >= frames[currentFrameIndex].duration) {
        elapsedTime = 0.0F;
        currentFrameIndex = (currentFrameIndex + 1) % frames.size();
    }
}

auto Animation::getCurrentFrame() const -> SDL_Texture* {
    if (frames.empty()) { return nullptr;
}
    return frames[currentFrameIndex].texture;
}
