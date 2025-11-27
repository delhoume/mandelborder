# Grid-Based Parallel Computation Implementation

## Overview
Successfully implemented a hierarchical calculator architecture that supports both single-threaded boundary tracing and grid-based sequential tile computation, setting the foundation for future parallel execution.

## Architecture

### Abstract Base Class: `MandelbrotCalculator`
- Defines the interface for all calculator implementations
- Contains pure virtual methods for computation, data access, and configuration
- Maintains `MAX_ITER` constant accessible to all implementations

### Concrete Implementations

#### 1. `BorderMandelbrotCalculator`
- Original boundary-tracing algorithm implementation
- Efficiently computes only boundary pixels and fills interior regions
- Optimized for sequential single-threaded execution

#### 2. `GridMandelbrotCalculator`
- Divides screen into configurable grid of tiles (e.g., 4×4, 16×16)
- Each tile uses a `BorderMandelbrotCalculator` instance
- Sequential execution through all tiles
- Composites tile data into unified buffer for rendering
- Progressive rendering as each tile completes

## Key Design Decisions

### 1. Hierarchy Over Coupling
Instead of adding grid logic throughout `MandelbrotApp`, we isolated it in a separate calculator class. This keeps the app code clean and makes the grid complexity self-contained.

### 2. Composition Over Code Duplication
`GridMandelbrotCalculator` reuses `BorderMandelbrotCalculator` instances rather than reimplementing the boundary-tracing algorithm.

### 3. Interface Preservation
The app continues using the same `MandelbrotCalculator` interface regardless of which implementation is active. No conditional logic scattered throughout the app.

### 4. Progressive Rendering
Each tile computation triggers the progress callback, allowing the user to see tiles being filled sequentially. The unified data buffer is composited after each tile update.

## Usage

### Default (Border Tracing)
```bash
./mandelbrot_sdl2
```

### Grid Calculator (4×4 tiles)
```bash
./mandelbrot_sdl2 --grid 4 4
```

### Grid Calculator (16×16 tiles - default)
```bash
./mandelbrot_sdl2 --grid
```

## File Structure

### New Files
- `mandelbrot_calculator.h` - Abstract base class
- `border_mandelbrot_calculator.h/cpp` - Boundary tracing implementation
- `grid_mandelbrot_calculator.h/cpp` - Grid-based implementation

### Modified Files
- `mandelbrot_app.h/cpp` - Uses abstract base class, factory method for calculator creation
- `main.cpp` - Command-line argument parsing for calculator selection
- `Makefile` - Updated to compile new source files

## Future Enhancements

### Parallel Execution (Ready for Implementation)
The grid structure is now in place. To add parallel execution:
1. Replace sequential tile loop with parallel execution (e.g., thread pool, OpenMP, std::async)
2. Add synchronization for the unified data buffer during compositing
3. Coordinate progress callbacks from multiple threads

### Data Copy Optimization
Currently, tile data is copied into the unified buffer. Potential optimizations:
- Direct rendering from tile buffers without copying
- Memory-mapped unified buffer where tiles write directly to their region
- Lazy compositing only when render is needed

### Additional Calculator Types
The architecture now supports adding:
- Brute-force calculator (compute every pixel)
- GPU-accelerated calculator
- Adaptive resolution calculator
- Different algorithms (e.g., level-set method, distance estimation)

## Testing Results

✅ Border calculator works identically to original implementation  
✅ Grid calculator produces correct visual output  
✅ Progressive rendering works correctly  
✅ Tiles compute sequentially in order  
✅ Command-line arguments parsed correctly  
✅ All code compiles without warnings

## Performance Notes

Grid calculator with 4×4 tiles:
- Completed in ~800ms for 800×600 resolution
- Each tile processes only its region's boundary pixels
- Overhead from tile subdivision is minimal
- Sequential execution ensures correctness before parallelization

## Next Steps

When ready for parallelization:
1. Add thread pool or OpenMP pragmas to tile computation loop
2. Implement mutex/atomic operations for unified buffer compositing
3. Add thread-safe progress callback coordination
4. Profile to find optimal tile count for your hardware
5. Consider work-stealing queue for dynamic load balancing
