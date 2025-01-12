#include "LightGrid.h"
#include <algorithm>
#include <cmath>
#include "Utils.h"

LightGrid::LightGrid(int width, int height, float scale, float offsetX, float offsetY)
    : width(width), height(height), scale(scale), offsetX(offsetX), offsetY(offsetY), lightGrid(width, std::vector<float>(height, 0.0f)), lightTexture(nullptr) {}

LightGrid::~LightGrid() {
    if (lightTexture != nullptr) {
        SDL_DestroyTexture(lightTexture);
    }
}

void LightGrid::propagateLight() {
    std::vector<std::vector<float>> newLightGrid = lightGrid;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float currentLight = lightGrid[x][y];
            if (currentLight > 0.0f) {
                float spreadAmount = currentLight * 0.25f;
                if (x > 0) newLightGrid[x - 1][y] += spreadAmount;
                if (x < width - 1) newLightGrid[x + 1][y] += spreadAmount;
                if (y > 0) newLightGrid[x][y - 1] += spreadAmount;
                if (y < height - 1) newLightGrid[x][y + 1] += spreadAmount;
                newLightGrid[x][y] -= spreadAmount * 4;
            }
        }
    }

    lightGrid = newLightGrid;
}

void LightGrid::dissipateLight() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            lightGrid[x][y] *= 0.99f;
        }
    }
}

void LightGrid::convertToTexture(SDL_Renderer* renderer) {
    if (lightTexture == nullptr) {
        lightTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    }

    SDL_SetRenderTarget(renderer, lightTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float intensity = std::clamp(lightGrid[x][y], 0.0f, 1.0f) * 255.0f;
            SDL_SetRenderDrawColor(renderer, intensity, intensity, intensity, 100);
            SDL_RenderPoint(renderer, x, y);
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);
}

void LightGrid::resetLightGrid() {
    for (auto& row : lightGrid) {
        std::fill(row.begin(), row.end(), 0.0f);
    }
}

void LightGrid::addPointLightSource(int x, int y, float intensity, SDL_Color color) {
    SDL_FPoint screenPos = Box2DToSDL(b2Vec2{static_cast<float>(x), static_cast<float>(y)}, scale, offsetX, offsetY, width, height);

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            float distance = std::sqrt((screenPos.x - i) * (screenPos.x - i) + (screenPos.y - j) * (screenPos.y - j));
            float lightIntensity = std::max(0.0f, intensity / (1.0f + (distance / 10.0f) * (distance / 10.0f)));
            lightGrid[i][j] += lightIntensity;
        }
    }
}

SDL_Texture* LightGrid::getLightTexture() const {
    return lightTexture;
}
