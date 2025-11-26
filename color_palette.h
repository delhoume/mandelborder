#ifndef COLOR_PALETTE_H
#define COLOR_PALETTE_H

#include <SDL2/SDL.h>
#include <vector>

/**
 * Abstract base class for color palettes used to visualize the Mandelbrot set.
 * Subclasses implement different color generation algorithms.
 */
class ColorPalette
{
public:
    virtual ~ColorPalette() = default;

    /**
     * Get the color for a given iteration count.
     * @param iteration The iteration count (0 to maxIterations-1)
     * @return SDL_Color structure with RGBA values
     */
    virtual SDL_Color getColor(int iteration) const = 0;

    /**
     * Get the maximum number of iterations this palette supports.
     */
    virtual int getMaxIterations() const = 0;
};

/**
 * A palette that uses cosine waves to generate smooth color transitions.
 * This encapsulates the original palette generation behavior.
 */
class CosineWavePalette : public ColorPalette
{
private:
    std::vector<SDL_Color> palette;
    int maxIterations;

    void generatePalette();

public:
    explicit CosineWavePalette(int maxIter);

    SDL_Color getColor(int iteration) const override;
    int getMaxIterations() const override { return maxIterations; }
};

#endif // COLOR_PALETTE_H
