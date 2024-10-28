#include "Animation.h"

Animation::Animation()
    : currentFrameIndex(0), elapsedTime(0.0f) {}

Animation::~Animation() {
    for (auto& frame : frames) {
        SDL_DestroyTexture(frame.texture);
    }
}

void Animation::addFrame(SDL_Texture* texture, int duration) {
    frames.push_back({texture, duration});
}

void Animation::update(float deltaTime) {
    if (frames.empty()) return;

    elapsedTime += deltaTime;
    if (elapsedTime >= frames[currentFrameIndex].duration) {
        elapsedTime = 0.0f;
        currentFrameIndex = (currentFrameIndex + 1) % frames.size();
    }
}

SDL_Texture* Animation::getCurrentFrame() const {
    if (frames.empty()) return nullptr;
    return frames[currentFrameIndex].texture;
}
