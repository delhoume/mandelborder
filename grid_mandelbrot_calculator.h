#pragma once

#include "mandelbrot_calculator.h"
#include "border_mandelbrot_calculator.h"
#include <vector>
#include <memory>
#include <functional>

// Grid-based implementation that divides computation into tiles
class GridMandelbrotCalculator : public MandelbrotCalculator
{
public:
    GridMandelbrotCalculator(int width, int height, int gridRows, int gridCols);

    void updateBounds(double cre, double cim, double diam) override;
    void updateBoundsExplicit(double minR, double minI, double maxR, double maxI) override;
    void compute(std::function<void()> progressCallback) override;
    void reset() override;

    const std::vector<int> &getData() const override { return unifiedData; }
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }

    double getCre() const override { return cre; }
    double getCim() const override { return cim; }
    double getDiam() const override { return diam; }
    double getMinR() const override { return minr; }
    double getMinI() const override { return mini; }
    double getStepR() const override { return stepr; }
    double getStepI() const override { return stepi; }

    void setSpeedMode(bool mode) override;
    bool getSpeedMode() const override { return speedMode; }
    
    void setVerboseMode(bool mode) override;
    bool getVerboseMode() const override { return verboseMode; }

private:
    int width;
    int height;
    int gridRows;
    int gridCols;

    double cre, cim, diam;
    double minr, mini, maxr, maxi;
    double stepr, stepi;

    bool speedMode;
    bool verboseMode;

    std::vector<std::unique_ptr<BorderMandelbrotCalculator>> tiles;
    std::vector<int> unifiedData;

    // Helper structures to track tile geometry
    struct TileInfo
    {
        int startX, startY;  // Starting pixel position
        int width, height;   // Tile dimensions in pixels
        double minR, minI;   // Complex plane bounds
        double maxR, maxI;
    };
    std::vector<TileInfo> tileInfos;

    void calculateTileGeometry();
    void compositeData();
};
