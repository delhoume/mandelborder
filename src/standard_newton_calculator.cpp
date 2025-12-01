#include "standard_newton_calculator.h"
#include <cmath>
#include <memory>

typedef struct {
  double x;
  double y;
} vec2;

const vec2 reminusone = {-1.0, 0.0};

vec2 complexMultiply(vec2 a, vec2 b) {
  return {a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x};
}

vec2 complexDivide(vec2 a, vec2 b) {
  double denom = b.x * b.x + b.y * b.y;
  return {(a.x * b.x + a.y * b.y) / denom, (a.y * b.x - a.x * b.y) / denom};
}

float complexDistance(vec2 a, vec2 b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

double complexDot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }

vec2 complexScale(double factor, vec2 z) {
  z.x *= factor;
  z.y *= factor;
  return z;
}
vec2 complexAdd(vec2 a, vec2 b) { return {a.x + b.x, a.y + b.y}; }
vec2 complexMinus(vec2 a, vec2 b) { return {a.x - b.x, a.y - b.y}; }

vec2 derivative(vec2 z) {
  // derivative of z^3-1 which is 3z^2
  return complexScale(3., complexMultiply(z, z));
}

// roots

const vec2 roots[3] = {{1, 0.00001}, {-.5, .86603}, {-.5, -.8660}};
const int nroots = 3;

vec2 function(vec2 z) {
  // z^3-1
  return complexAdd(reminusone, complexMultiply(z, complexMultiply(z, z)));
}

vec2 slow_newton(vec2 z) {
  return complexMinus(z, complexDivide(function(z), derivative(z)));
}

StandardNewtonCalculator::StandardNewtonCalculator(int w, int h)
    : StorageMandelbrotCalculator(w, h) {}

const double THRESHOLD = .00001;
const int BAND_SIZE = MandelbrotCalculator::MAX_ITER / nroots;
int StandardNewtonCalculator::iterate(double x, double y) {
  vec2 z = {x, y};
  for (int i = 0; i < MandelbrotCalculator::MAX_ITER; ++i) {
    z = slow_newton(z);
    for (int r = 0; r < nroots; ++r) {
      vec2 diff = complexMinus(z, roots[r]);
      double dist = complexDot(diff, diff);
      if (dist <= THRESHOLD) {
        return (r * BAND_SIZE) +
               BAND_SIZE * (0.75 +
                     0.25 * cos(0.25 *
                                (float(i) - log2(log(dist) / log(THRESHOLD)))));
      }
    }
  }
  return MAX_ITER;
}

void StandardNewtonCalculator::compute(std::function<void()> progressCallback) {
  int processed = 0;
  for (int y = 0; y < height; ++y) {
    double cy = mini + y * stepi;
    for (int x = 0; x < width; ++x) {
      double cx = minr + x * stepr;
      data[y * width + x] = iterate(cx, cy);
      processed++;
    }

    // Update display periodically (skip in speed mode)
    if (!speedMode && processed % (width * 10) == 0) // Update every 10 lines
    {
      if (progressCallback)
        progressCallback();
    }
  }
}
