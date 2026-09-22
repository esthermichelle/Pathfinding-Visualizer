#include "GridModel.hpp"

GridModel::GridModel(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    grid.resize(rows);
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

bool GridModel::inBounds(Position pos) const
{
    return pos.row >= 0 && pos.row < rows && pos.col >= 0 && pos.col < cols;
}

Node &GridModel::getNode(Position pos)
{
    return grid[pos.row][pos.col];
}

NodeState GridModel::getNodeState(Position pos) const
{
    return grid[pos.row][pos.col].state;
}

void GridModel::setNodeState(Position pos, NodeState state)
{
    grid[pos.row][pos.col].state = state;
}

bool GridModel::isWall(Position pos) const
{
    return grid[pos.row][pos.col].state == NodeState::WALL;
}

void GridModel::resetStates()
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (grid[r][c].state != NodeState::WALL &&
                grid[r][c].state != NodeState::START &&
                grid[r][c].state != NodeState::TARGET)
            {
                grid[r][c].state = NodeState::EMPTY;
            }
            grid[r][c].parent = {-1, -1};
            grid[r][c].gCost = std::numeric_limits<double>::infinity();
            grid[r][c].hCost = std::numeric_limits<double>::infinity();
        }
    }
}

