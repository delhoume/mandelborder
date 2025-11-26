#include "color_palette.h"
#include <cmath>

CosineWavePalette::CosineWavePalette(int maxIter)
    : maxIterations(maxIter)
{
    generatePalette();
}

void CosineWavePalette::generatePalette()
{
    palette.resize(maxIterations);
    double freq = 2.0 * M_PI / maxIterations;
    const int amplitude = 110;
    const int base0 = amplitude;
    const int base1 = 255 - amplitude - 1;
    
    for (int c = 0; c < maxIterations; ++c)
    {
        if (c % 2 == 0)
        {
            palette[c].r = static_cast<Uint8>(base0 - amplitude * std::cos(c * freq * 1));
            palette[c].g = static_cast<Uint8>(base0 - amplitude * std::cos(c * freq * 3));
            palette[c].b = static_cast<Uint8>(base0 - amplitude * std::cos(c * freq * 5));
        }
        else
        {
            palette[c].r = static_cast<Uint8>(base1 - amplitude * std::cos(c * freq * 1));
            palette[c].g = static_cast<Uint8>(base1 - amplitude * std::cos(c * freq * 3));
            palette[c].b = static_cast<Uint8>(base1 - amplitude * std::cos(c * freq * 5));
        }
        palette[c].a = 255;
    }
}

SDL_Color CosineWavePalette::getColor(int iteration) const
{
    if (iteration >= 0 && iteration < maxIterations)
    {
        return palette[iteration];
    }
    
    // Return black for out-of-range values
    return {0, 0, 0, 255};
}
