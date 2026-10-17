#include "GridRenderer.hpp"

GridRenderer::GridRenderer(int offsetX, int offsetY, int cellSize)
    : offsetX(offsetX), offsetY(offsetY), cellSize(cellSize)
{
}

Color GridRenderer::stateColor(NodeState state) const
{
    switch (state)
    {
    case NodeState::EMPTY:
        return Color{44, 47, 68, 255};      // dark slate
    case NodeState::WALL:
        return Color{20, 20, 35, 255};       // very dark navy
    case NodeState::START:
        return Color{0, 212, 170, 255};      // mint green
    case NodeState::TARGET:
        return Color{255, 107, 107, 255};    // coral red
    case NodeState::FRONTIER:
        return Color{116, 185, 255, 255};    // sky blue
    case NodeState::VISITED:
        return Color{162, 155, 254, 255};    // soft purple
    case NodeState::PATH:
        return Color{255, 217, 61, 255};     // golden yellow
    default:
        return DARKGRAY;
    }
}

void GridRenderer::draw(const GridModel &model) const
{
    int rows = model.getRows();
    int cols = model.getCols();

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            int x = offsetX + c * cellSize;
            int y = offsetY + r * cellSize;

            NodeState state = model.getNodeState({r, c});
            Color col = stateColor(state);

            DrawRectangle(x, y, cellSize - 1, cellSize - 1, col);

            // Draw labels on start/target cells
            if (state == NodeState::START)
            {
                int fontSize = cellSize / 2;
                const char *label = "S";
                int tw = MeasureText(label, fontSize);
                DrawText(label, x + (cellSize - 1 - tw) / 2,
                         y + (cellSize - 1 - fontSize) / 2,
                         fontSize, Color{0, 0, 0, 200});
            }
            else if (state == NodeState::TARGET)
            {
                int fontSize = cellSize / 2;
                const char *label = "T";
                int tw = MeasureText(label, fontSize);
                DrawText(label, x + (cellSize - 1 - tw) / 2,
                         y + (cellSize - 1 - fontSize) / 2,
                         fontSize, Color{0, 0, 0, 200});
            }
        }
    }
}

Position GridRenderer::screenToGrid(int screenX, int screenY, const GridModel &model) const
{
    int col = (screenX - offsetX) / cellSize;
    int row = (screenY - offsetY) / cellSize;

    Position pos{row, col};
    if (model.inBounds(pos) && screenX >= offsetX && screenY >= offsetY)
        return pos;

    return {-1, -1};
}

int GridRenderer::gridPixelWidth(const GridModel &model) const
{
    return model.getCols() * cellSize;
}

int GridRenderer::gridPixelHeight(const GridModel &model) const
{
    return model.getRows() * cellSize;
}
