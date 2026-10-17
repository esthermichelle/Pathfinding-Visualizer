#pragma once

#include "GridModel.hpp"
#include "raylib.h"

class GridRenderer
{
public:
    GridRenderer(int offsetX, int offsetY, int cellSize);

    void draw(const GridModel &model) const;

    // Convert screen pixel to grid position; returns {-1,-1} if out of bounds
    Position screenToGrid(int screenX, int screenY, const GridModel &model) const;

    int getOffsetX() const { return offsetX; }
    int getOffsetY() const { return offsetY; }
    int getCellSize() const { return cellSize; }

    // Total pixel dimensions of the drawn grid
    int gridPixelWidth(const GridModel &model) const;
    int gridPixelHeight(const GridModel &model) const;

private:
    int offsetX;
    int offsetY;
    int cellSize;

    Color stateColor(NodeState state) const;
};
