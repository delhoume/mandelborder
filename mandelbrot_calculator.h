#pragma once

#include <vector>
#include <functional>

class MandelbrotCalculator
{
public:
    MandelbrotCalculator(int width, int height);

    void updateBounds(double cre, double cim, double diam);
    void compute(std::function<void()> progressCallback);
    void reset();

    const std::vector<int> &getData() const { return data; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Getters for current view parameters
    double getCre() const { return cre; }
    double getCim() const { return cim; }
    double getDiam() const { return diam; }
    double getMinR() const { return minr; }
    double getMinI() const { return mini; }
    double getStepR() const { return stepr; }
    double getStepI() const { return stepi; }

    void setSpeedMode(bool mode) { speedMode = mode; }
    bool getSpeedMode() const { return speedMode; }

private:
    int width;
    int height;
    static constexpr int MAX_ITER = 768;

    double cre, cim, diam;
    double minr, mini, maxr, maxi;
    double stepr, stepi;

    std::vector<int> data;
    std::vector<unsigned char> done;
    std::vector<unsigned> queue;
    unsigned queueHead, queueTail;

    bool speedMode;

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
