#pragma once

#include <SDL3/SDL.h>
#include <string>

/**
 * Screenshot utility for regression testing
 */
class ScreenshotCapture {
public:
    /**
     * Save a screenshot of the current renderer to a PNG file
     * @param renderer The SDL renderer to capture
     * @param filename The output filename (without extension)
     * @param width The window width
     * @param height The window height
     * @return true if successful, false otherwise
     */
    static bool saveScreenshot(SDL_Renderer* renderer, const std::string& filename, int width, int height);

private:
    /**
     * Save raw pixel data as PNG
     * @param pixels Raw pixel data in RGBA format
     * @param width Image width
     * @param height Image height
     * @param filename Output filename
     * @return true if successful
     */
    static bool savePNG(const void* pixels, int width, int height, const std::string& filename);
};