#include "gradient.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdlib>

CosineGradient::CosineGradient(int base, int amplitude, double freqR, double freqG, double freqB)
    : base(base), amplitude(amplitude), freqR(freqR), freqG(freqG), freqB(freqB)
{
}

SDL_Color CosineGradient::getColor(double t) const
{
    // t is 0..1
    // angle = t * 2 * PI
    
    double r = base - amplitude * std::cos(t * 2 * M_PI * freqR);
    double g = base - amplitude * std::cos(t * 2 * M_PI * freqG);
    double b = base - amplitude * std::cos(t * 2 * M_PI * freqB);

    auto clamp = [](double val) -> Uint8 {
        if (val < 0) return 0;
        if (val > 255) return 255;
        return static_cast<Uint8>(val);
    };

    return {clamp(r), clamp(g), clamp(b), 255};
}

std::unique_ptr<CosineGradient> CosineGradient::createRandom()
{
    // Use prime numbers for frequencies to avoid repeating patterns too quickly
    const std::vector<double> primes = {1.0, 2.0, 3.0, 5.0, 7.0, 11.0, 13.0};
    
    double freqR = primes[rand() % primes.size()];
    double freqG = primes[rand() % primes.size()];
    double freqB = primes[rand() % primes.size()];

    // Random base and amplitude
    // Ensure base + amplitude <= 255 and base - amplitude >= 0
    // Let's pick amplitude first, say between 50 and 127
    int amplitude = 50 + rand() % 78;
    
    // Then base must be at least amplitude and at most 255 - amplitude
    int minBase = amplitude;
    int maxBase = 255 - amplitude;
    int base = minBase + rand() % (maxBase - minBase + 1);

    return std::make_unique<CosineGradient>(base, amplitude, freqR, freqG, freqB);
}
