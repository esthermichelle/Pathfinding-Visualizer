#include "GridRenderer.hpp"

GridRenderer::GridRenderer(int offsetX, int offsetY, int cellSize, Font font)
    : offsetX(offsetX), offsetY(offsetY), cellSize(cellSize), font(font)
{
}

Color GridRenderer::stateColor(NodeState state) const
{
    switch (state)
    {
    case NodeState::EMPTY:
        return WHITE;
    case NodeState::WALL:
        return Color{140, 140, 150, 255};    // medium gray
    case NodeState::START:
        return Color{76, 175, 80, 255};      // muted green
    case NodeState::TARGET:
        return Color{211, 78, 78, 255};      // muted red
    case NodeState::FRONTIER:
        return Color{187, 222, 251, 255};    // pale blue
    case NodeState::VISITED:
        return Color{207, 216, 220, 255};    // light blue-gray
    case NodeState::PATH:
        return Color{255, 202, 40, 255};     // soft amber
    default:
        return LIGHTGRAY;
    }
}

void GridRenderer::draw(const GridModel &model) const
{
    int rows = model.getRows();
    int cols = model.getCols();

    // Draw cell fills
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            int x = offsetX + c * cellSize;
            int y = offsetY + r * cellSize;

            NodeState state = model.getNodeState({r, c});
            Color col = stateColor(state);

            DrawRectangle(x, y, cellSize, cellSize, col);
        }
    }

    // Draw grid lines (very light gray)
    Color gridLine = Color{225, 225, 230, 255};
    int totalW = cols * cellSize;
    int totalH = rows * cellSize;

    for (int r = 0; r <= rows; r++)
    {
        int y = offsetY + r * cellSize;
        DrawLine(offsetX, y, offsetX + totalW, y, gridLine);
    }
    for (int c = 0; c <= cols; c++)
    {
        int x = offsetX + c * cellSize;
        DrawLine(x, offsetY, x, offsetY + totalH, gridLine);
    }

    // Draw labels on start/target cells
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            NodeState state = model.getNodeState({r, c});
            if (state != NodeState::START && state != NodeState::TARGET)
                continue;

            int x = offsetX + c * cellSize;
            int y = offsetY + r * cellSize;
            float fontSize = static_cast<float>(cellSize) * 0.64f;
            const char *label = (state == NodeState::START) ? "S" : "T";
            float tw = MeasureTextEx(font, label, fontSize, 0.0f).x;
            DrawTextEx(font, label,
                       Vector2{static_cast<float>(x) + (static_cast<float>(cellSize) - tw) / 2.0f,
                               static_cast<float>(y) + (static_cast<float>(cellSize) - fontSize) / 2.0f},
                       fontSize, 0.0f, WHITE);
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
