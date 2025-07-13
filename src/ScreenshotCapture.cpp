#include "ScreenshotCapture.h"
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <memory>

bool ScreenshotCapture::saveScreenshot(SDL_Renderer* renderer, const std::string& filename, int width, int height) {
    if (!renderer) {
        spdlog::error("ScreenshotCapture: Invalid renderer");
        return false;
    }

    // Read pixels from the renderer directly to a surface
    SDL_Surface* surface = SDL_RenderReadPixels(renderer, nullptr);
    if (!surface) {
        spdlog::error("ScreenshotCapture: Failed to read pixels: {}", SDL_GetError());
        return false;
    }

    // Save the surface as PNG
    std::string fullFilename = filename + ".png";
    if (!IMG_SavePNG(surface, fullFilename.c_str())) {
        spdlog::error("ScreenshotCapture: Failed to save PNG {}: {}", fullFilename, SDL_GetError());
        SDL_DestroySurface(surface);
        return false;
    }

    SDL_DestroySurface(surface);
    spdlog::info("ScreenshotCapture: Screenshot saved to {}", fullFilename);
    return true;
}

bool ScreenshotCapture::savePNG(const void* pixels, int width, int height, const std::string& filename) {
    // Create surface from pixel data
    // Calculate pitch (bytes per row)
    int pitch = width * 4; // 4 bytes per pixel for RGBA
    
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        width, 
        height, 
        SDL_PIXELFORMAT_RGBA8888,
        const_cast<void*>(pixels), 
        pitch
    );
    
    if (!surface) {
        spdlog::error("ScreenshotCapture: Failed to create surface from pixels: {}", SDL_GetError());
        return false;
    }

    // Save as PNG
    bool success = IMG_SavePNG(surface, filename.c_str());
    SDL_DestroySurface(surface);
    
    if (!success) {
        spdlog::error("ScreenshotCapture: Failed to save PNG {}: {}", filename, SDL_GetError());
    }
    
    return success;
}