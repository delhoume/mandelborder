#pragma once

#include "mandelbrot_calculator.h"
#include <vector>
#include <functional>

// Boundary-tracing implementation of Mandelbrot calculator
class BorderMandelbrotCalculator : public MandelbrotCalculator
{
public:
    BorderMandelbrotCalculator(int width, int height);

    void updateBounds(double cre, double cim, double diam) override;
    void updateBoundsExplicit(double minR, double minI, double maxR, double maxI) override;
    void compute(std::function<void()> progressCallback) override;
    void reset() override;

    const std::vector<int> &getData() const override { return data; }
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }

    // Getters for current view parameters
    double getCre() const override { return cre; }
    double getCim() const override { return cim; }
    double getDiam() const override { return diam; }
    double getMinR() const override { return minr; }
    double getMinI() const override { return mini; }
    double getStepR() const override { return stepr; }
    double getStepI() const override { return stepi; }

    void setSpeedMode(bool mode) override { speedMode = mode; }
    bool getSpeedMode() const override { return speedMode; }
    
    void setVerboseMode(bool mode) override { verboseMode = mode; }
    bool getVerboseMode() const override { return verboseMode; }

private:
    int width;
    int height;

    double cre, cim, diam;
    double minr, mini, maxr, maxi;
    double stepr, stepi;

    std::vector<int> data;
    std::vector<unsigned char> done;
    std::vector<unsigned> queue;
    unsigned queueHead, queueTail;

    bool speedMode;
    bool verboseMode;

    enum Flags
    {
        LOADED = 1,
        QUEUED = 2
    };

    int iterate(double x, double y);
    void addQueue(unsigned p);
    int load(unsigned p);
    void scan(unsigned p);
};
