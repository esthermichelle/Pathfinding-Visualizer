#include "GridModel.hpp"

GridModel::GridModel(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
        for (int r = 0; r < rows; r++)
    {
        grid[r].resize(cols);

        for (int c = 0; c < cols; c++)
        {
            grid[r][c].pos = {r, c};
        }
    }
}

int GridModel::getRows() const
{
    return rows;
}

int GridModel::getCols() const
{
    return cols;
}
