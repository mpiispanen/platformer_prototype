#include "Animation.h"

Animation::Animation() : currentFrameIndex(0), currentTime(0.0F), flip(SDL_FLIP_NONE), isLooping(true), isCompleted(false) {}

Animation::~Animation() {
    for (auto& frame : frames) {
        SDL_DestroyTexture(frame.texture);
    }
}

void Animation::addFrame(SDL_Texture* texture, int duration) {
    frames.push_back({texture, duration});
}

void Animation::update(float deltaTime) {
    if (frames.empty()) { return; }

    const uint32_t SECOND_IN_MILLISECONDS = 1000;
    currentTime += deltaTime * SECOND_IN_MILLISECONDS;
    if (currentTime >= frames[currentFrameIndex].duration) {
        currentTime = 0.0F;
        if (isLooping) {
            currentFrameIndex = (currentFrameIndex + 1) % frames.size();
        } else {
            if (currentFrameIndex < frames.size() - 1) {
                currentFrameIndex++;
            } else {
                isCompleted = true;
            }
        }
    }
}

auto Animation::getCurrentFrame() const -> SDL_Texture* {
    if (frames.empty()) { return nullptr; }
    return frames[currentFrameIndex].texture;
}

auto Animation::getFlip() const -> SDL_FlipMode {
    return flip;
}

void Animation::setFlip(SDL_FlipMode flip) {
    this->flip = flip;
}

void Animation::setLooping(bool looping) {
    isLooping = looping;
}

void Animation::reset() {
    currentFrameIndex = 0;
    currentTime = 0.0F;
    isCompleted = false;
}

void Animation::setName(const std::string& name) {
    this->name = name;
}

auto Animation::getName() const -> std::string {
    return name;
}

auto Animation::getCurrentFrameIndex() const -> int {
    return currentFrameIndex;
}

auto Animation::getTotalFrames() const -> int {
    return frames.size();
}

bool Animation::hasCompleted() const {
    return isCompleted;
}
