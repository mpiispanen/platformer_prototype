#include "Animation.h"

Animation::Animation() : currentFrameIndex(0), currentTime(0.0F), flip(SDL_FLIP_NONE) {}

Animation::~Animation() {
    for (auto& frame : frames) {
        SDL_DestroyTexture(frame.texture);
    }
}

void Animation::addFrame(SDL_Texture* texture, int duration) {
    frames.push_back({texture, duration});
}

void Animation::update(float deltaTime) {
    if (frames.empty()) { return;
}

    const uint32_t SECOND_IN_MILLISECONDS = 1000;
    currentTime += deltaTime * SECOND_IN_MILLISECONDS;
    if (currentTime >= frames[currentFrameIndex].duration) {
        currentTime = 0.0F;
        currentFrameIndex = (currentFrameIndex + 1) % frames.size();
    }
}

auto Animation::getCurrentFrame() const -> SDL_Texture* {
    if (frames.empty()) { return nullptr;
}
    return frames[currentFrameIndex].texture;
}

auto Animation::getFlip() const -> SDL_FlipMode {
    return flip;
}

void Animation::setFlip(SDL_FlipMode flip) {
    this->flip = flip;
}
