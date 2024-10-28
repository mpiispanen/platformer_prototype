#include "Animation.h"

Animation::Animation()
    : currentFrameIndex(0), elapsedTime(0.0F) {}

Animation::~Animation() {
    for (auto& frame : frames) {
        SDL_DestroyTexture(frame.texture);
    }
}

void Animation::addFrame(SDL_Texture* texture, int duration) {
    frames.push_back({texture, duration});
}

void Animation::update(float deltaTime) {
    if (frames.empty()) {
        return;
    }

    elapsedTime += deltaTime;
    if (elapsedTime >= frames[currentFrameIndex].duration) {
        elapsedTime = 0.0F;
        currentFrameIndex = (currentFrameIndex + 1) % frames.size();
    }
}

auto Animation::getCurrentFrame() const -> SDL_Texture* {
    if (frames.empty()) return nullptr;
    return frames[currentFrameIndex].texture;
}
