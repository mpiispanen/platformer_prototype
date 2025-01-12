#pragma once

#include <SDL3/SDL.h>
#include <vector>

class LightGrid {
public:
    LightGrid(int width, int height, float scale, float offsetX, float offsetY);
    ~LightGrid();

    void propagateLight();
    void dissipateLight();
    void convertToTexture(SDL_Renderer* renderer);
    void resetLightGrid();
    void addPointLightSource(int x, int y, float intensity, SDL_Color color);
    SDL_Texture* getLightTexture() const;

private:
    int width;
    int height;
    float scale;
    float offsetX;
    float offsetY;
    std::vector<std::vector<float>> lightGrid;
    SDL_Texture* lightTexture;
};
