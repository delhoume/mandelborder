#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "mandelbrot_calculator.h"

class MandelbrotApp
{
public:
    MandelbrotApp(int width, int height);
    ~MandelbrotApp();

    void run();

private:
    int width;
    int height;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    std::unique_ptr<MandelbrotCalculator> calculator;
    std::vector<SDL_Color> palette;

    void initSDL();
    void generatePalette();
    void render();

    // Interaction helpers
    SDL_Rect calculateSelectionRect(int startX, int startY, int endX, int endY, bool centerBased);
    void zoomToRegion(int x1, int y1, int x2, int y2);
    void zoomToRect(int x1, int y1, int x2, int y2, bool inverse = false);
    void animateRectToRect(int startX, int startY, int startWidth, int startHeight,
                           int endX, int endY, int endWidth, int endHeight,
                           int steps = 15, int frameDelay = 16);
    void resetZoom();
};
