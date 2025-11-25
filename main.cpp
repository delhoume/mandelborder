#include "mandelbrot_app.h"
#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        // Default resolution 800x600
        MandelbrotApp app(800, 600);
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
