#pragma once

#include "GridModel.hpp"
#include <random>

// Generates mazes using randomized DFS (recursive backtracker)
// with iterative stack to avoid stack overflow on large grids.
//
// The algorithm works on a conceptual grid where every other cell
// is a potential passage, and cells in between are walls. This
// produces a proper maze with single-cell-wide corridors.
class MazeGenerator
{
public:
    // Generate a maze on the given grid, preserving start and target positions.
    // Uses the provided seed for reproducibility (0 = random seed).
    static void generate(GridModel &grid, Position start, Position target,
                         unsigned int seed = 0);
};
