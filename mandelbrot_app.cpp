#include "mandelbrot_app.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>

MandelbrotApp::MandelbrotApp(int w, int h)
    : width(w), height(h), window(nullptr), renderer(nullptr), texture(nullptr)
{
    calculator = std::make_unique<MandelbrotCalculator>(width, height);
    initSDL();
    generatePalette();
}

MandelbrotApp::~MandelbrotApp()
{
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void MandelbrotApp::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error(std::string("SDL initialization failed: ") + SDL_GetError());
    }

    window = SDL_CreateWindow(
        "Mandelbrot Set - Boundary Tracing",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);

    if (!window)
    {
        SDL_Quit();
        throw std::runtime_error(std::string("Window creation failed: ") + SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(std::string("Renderer creation failed: ") + SDL_GetError());
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
                                SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!texture)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(std::string("Texture creation failed: ") + SDL_GetError());
    }
}

void MandelbrotApp::generatePalette()
{
    palette.resize(256);
    for (int c = 0; c < 256; ++c)
    {
        palette[c].r = static_cast<Uint8>(128 - 127 * std::cos(c * 0.01227 * 1));
        palette[c].g = static_cast<Uint8>(128 - 127 * std::cos(c * 0.01227 * 3));
        palette[c].b = static_cast<Uint8>(128 - 127 * std::cos(c * 0.01227 * 5));
        palette[c].a = 255;
    }
}

void MandelbrotApp::render()
{
    Uint32 *pixels;
    int pitch;

    SDL_LockTexture(texture, nullptr, (void **)&pixels, &pitch);

    const auto &data = calculator->getData();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int p = y * width + x;
            int iter = data[p] % 256;
            SDL_Color color = palette[iter];
            pixels[y * (pitch / 4) + x] = (color.r << 16) | (color.g << 8) | color.b;
        }
    }

    SDL_UnlockTexture(texture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

SDL_Rect MandelbrotApp::calculateSelectionRect(int startX, int startY, int endX, int endY, bool centerBased)
{
    int dx = endX - startX;
    int dy = endY - startY;
    double aspectRatio = (double)width / height;
    int w, h, rectX, rectY;

    // Calculate raw dimensions
    if (centerBased)
    {
        w = std::abs(dx) * 2;
        h = std::abs(dy) * 2;
    }
    else
    {
        w = std::abs(dx);
        h = std::abs(dy);
    }

    // Apply aspect ratio correction
    if (w / aspectRatio > h)
        h = (int)(w / aspectRatio);
    else
        w = (int)(h * aspectRatio);

    // Calculate top-left position
    if (centerBased)
    {
        rectX = startX - w / 2;
        rectY = startY - h / 2;
    }
    else
    {
        rectX = std::min(startX, endX);
        rectY = std::min(startY, endY);
    }

    return {rectX, rectY, w, h};
}

void MandelbrotApp::resetZoom()
{
    calculator->updateBounds(-0.5, 0.0, 3.0);
}

void MandelbrotApp::zoomToRegion(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 || y1 == y2)
        return;

    // Ensure x1,y1 is top-left
    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);

    // Convert pixel coordinates to complex plane
    double minr = calculator->getMinR();
    double mini = calculator->getMinI();
    double stepr = calculator->getStepR();
    double stepi = calculator->getStepI();

    double re1 = minr + x1 * stepr;
    double im1 = mini + y1 * stepi;
    double re2 = minr + x2 * stepr;
    double im2 = mini + y2 * stepi;

    double new_cre = (re1 + re2) / 2.0;
    double new_cim = (im1 + im2) / 2.0;
    double new_diam = std::max(re2 - re1, im2 - im1);

    calculator->updateBounds(new_cre, new_cim, new_diam);

    std::cout << "Zoomed to: center=(" << calculator->getCre() << ", " << calculator->getCim()
              << "), diameter=" << calculator->getDiam() << std::endl;
}

void MandelbrotApp::animateRectToRect(int startX, int startY, int startWidth, int startHeight,
                                      int endX, int endY, int endWidth, int endHeight,
                                      int steps, int frameDelay)
{
    // Skip animation in speed mode
    if (calculator->getSpeedMode())
        return;

    // Animate rectangle transformation over specified number of steps
    for (int step = 0; step <= steps; ++step)
    {
        double t = (double)step / steps;

        // Interpolate rectangle dimensions
        int currentX = (int)(startX + (endX - startX) * t);
        int currentY = (int)(startY + (endY - startY) * t);
        int currentWidth = (int)(startWidth + (endWidth - startWidth) * t);
        int currentHeight = (int)(startHeight + (endHeight - startHeight) * t);

        // Redraw the current frame
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect = {currentX, currentY, currentWidth, currentHeight};
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderPresent(renderer);

        SDL_Delay(frameDelay);
    }
}

void MandelbrotApp::zoomToRect(int x1, int y1, int x2, int y2, bool inverse)
{
    if (inverse)
    {
        // Zoom OUT: animate full screen shrinking to rectangle
        animateRectToRect(0, 0, width, height, x1, y1, x2 - x1, y2 - y1);

        // Calculate scale and new center
        double scale = std::max((double)width / (x2 - x1), (double)height / (y2 - y1));
        int offsetX = (x1 + x2) / 2 - width / 2;
        int offsetY = (y1 + y2) / 2 - height / 2;

        double new_cre = calculator->getCre() + offsetX * calculator->getStepR() * scale;
        double new_cim = calculator->getCim() + offsetY * calculator->getStepI() * scale;
        double new_diam = calculator->getDiam() * scale;
        calculator->updateBounds(new_cre, new_cim, new_diam);
    }
    else
    {
        // Zoom IN: animate rectangle expanding to full screen
        animateRectToRect(x1, y1, x2 - x1, y2 - y1, 0, 0, width, height);
        zoomToRegion(x1, y1, x2, y2);
    }

    calculator->reset();
    calculator->compute([this]()
                        { this->render(); });
    render();
}

void MandelbrotApp::run()
{
    calculator->compute([this]()
                        { this->render(); });
    render();

    std::cout << "Press ESC to quit, SPACE to recompute, R to reset zoom, S to toggle speed mode" << std::endl;
    std::cout << "Click and drag to zoom into a region (SHIFT to zoom out, CTRL for center-based)" << std::endl;

    bool running = true;
    SDL_Event event;

    bool dragging = false;
    int dragStartX = 0, dragStartY = 0;
    int dragEndX = 0, dragEndY = 0;
    int currentMouseX = 0, currentMouseY = 0;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if (dragging)
                    {
                        // Cancel drag operation
                        dragging = false;
                        render(); // Redraw without selection rectangle
                    }
                    else
                    {
                        running = false;
                    }
                }
                else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
                {
                    calculator->reset();
                    calculator->compute([this]()
                                        { this->render(); });
                    render();
                }
                else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
                {
                    resetZoom();
                    calculator->reset();
                    calculator->compute([this]()
                                        { this->render(); });
                    render();
                }
                else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
                {
                    bool mode = !calculator->getSpeedMode();
                    calculator->setSpeedMode(mode);
                    std::cout << "Speed mode: " << (mode ? "ON" : "OFF") << std::endl;
                }
                else if (dragging && (event.key.keysym.sym == SDLK_LCTRL ||
                                      event.key.keysym.sym == SDLK_RCTRL))
                {
                    // CTRL pressed/released during drag - redraw rectangle immediately
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                    SDL_Keymod modState = SDL_GetModState();
                    bool ctrlPressed = (modState & KMOD_CTRL) != 0;

                    SDL_Rect rect = calculateSelectionRect(dragStartX, dragStartY, currentMouseX, currentMouseY, ctrlPressed);
                    SDL_RenderDrawRect(renderer, &rect);
                    SDL_RenderPresent(renderer);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    dragging = true;
                    dragStartX = event.button.x;
                    dragStartY = event.button.y;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT && dragging)
                {
                    dragging = false;
                    dragEndX = event.button.x;
                    dragEndY = event.button.y;

                    // Check if SHIFT is pressed for zoom out (inverse)
                    SDL_Keymod modState = SDL_GetModState();
                    bool shiftPressed = (modState & KMOD_SHIFT) != 0;

                    int x1, y1, x2, y2;

                    // Check if this was a click (no drag) vs a drag
                    int dx = dragEndX - dragStartX;
                    int dy = dragEndY - dragStartY;
                    int dragDistance = std::abs(dx) + std::abs(dy);

                    if (dragDistance < 5)
                    {
                        // Click with no drag - zoom 2x centered on click point
                        int w = width / 2;
                        int h = height / 2;

                        x1 = dragStartX - w / 2;
                        y1 = dragStartY - h / 2;
                        x2 = x1 + w;
                        y2 = y1 + h;
                    }
                    else
                    {
                        // Check if CTRL is pressed for center-based zoom
                        bool ctrlPressed = (modState & KMOD_CTRL) != 0;

                        SDL_Rect rect = calculateSelectionRect(dragStartX, dragStartY, dragEndX, dragEndY, ctrlPressed);
                        x1 = rect.x;
                        y1 = rect.y;
                        x2 = x1 + rect.w;
                        y2 = y1 + rect.h;
                    }

                    zoomToRect(x1, y1, x2, y2, shiftPressed);
                }
                break;

            case SDL_MOUSEMOTION:
                if (dragging)
                {
                    // Track current mouse position
                    currentMouseX = event.motion.x;
                    currentMouseY = event.motion.y;

                    // Draw selection rectangle with correct aspect ratio
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                    // Check if CTRL is pressed for center-based zoom
                    SDL_Keymod modState = SDL_GetModState();
                    bool ctrlPressed = (modState & KMOD_CTRL) != 0;

                    SDL_Rect rect = calculateSelectionRect(dragStartX, dragStartY, currentMouseX, currentMouseY, ctrlPressed);
                    SDL_RenderDrawRect(renderer, &rect);
                    SDL_RenderPresent(renderer);
                }
                break;
            }
        }

        SDL_Delay(16); // ~60 FPS
    }
}
