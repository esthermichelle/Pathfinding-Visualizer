#include "raylib.h"
#include "GridRenderer.hpp"
#include "VisualizerEngine.hpp"
#include "UIManager.hpp"

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;
static const int GRID_ROWS = 25;
static const int GRID_COLS = 40;
static const int CELL_SIZE = 25;
static const int GRID_OFFSET_X = 20;
static const int GRID_OFFSET_Y = 60;
static const int PANEL_WIDTH = 240;

static const Color BG_COLOR = WHITE;
static const Color TITLE_COLOR = {50, 50, 55, 255};
static const Color SUBTITLE_COLOR = {130, 130, 140, 255};

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pathfinding Visualizer");
    SetTargetFPS(60);

    VisualizerEngine engine(GRID_ROWS, GRID_COLS);
    GridRenderer renderer(GRID_OFFSET_X, GRID_OFFSET_Y, CELL_SIZE);
    int panelX = GRID_OFFSET_X + GRID_COLS * CELL_SIZE + 10;
    UIManager ui(panelX, 0, SCREEN_WIDTH - panelX, SCREEN_HEIGHT);

    bool draggingStart = false;
    bool draggingTarget = false;

    while (!WindowShouldClose())
    {
        // --- INPUT ---
        Vector2 mouse = GetMousePosition();
        int mx = static_cast<int>(mouse.x);
        int my = static_cast<int>(mouse.y);
        Position gridPos = renderer.screenToGrid(mx, my, engine.getGrid());
        bool onGrid = (gridPos.row != -1 && gridPos.col != -1);

        // UI panel input (buttons)
        ui.handleInput(engine);

        // Grid mouse input
        EngineState state = engine.getState();
        bool canEdit = (state == EngineState::IDLE || state == EngineState::FINISHED);

        if (onGrid && canEdit)
        {
            // Start dragging start/target
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                NodeState clicked = engine.getGrid().getNodeState(gridPos);
                if (clicked == NodeState::START)
                {
                    draggingStart = true;
                }
                else if (clicked == NodeState::TARGET)
                {
                    draggingTarget = true;
                }
                else if (clicked == NodeState::EMPTY || clicked == NodeState::VISITED ||
                         clicked == NodeState::FRONTIER || clicked == NodeState::PATH)
                {
                    engine.getGrid().setNodeState(gridPos, NodeState::WALL);
                }
            }

            // Dragging or continuous wall painting
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                if (draggingStart)
                {
                    engine.moveStartTo(gridPos);
                }
                else if (draggingTarget)
                {
                    engine.moveTargetTo(gridPos);
                }
                else
                {
                    NodeState cellState = engine.getGrid().getNodeState(gridPos);
                    if (cellState == NodeState::EMPTY || cellState == NodeState::VISITED ||
                        cellState == NodeState::FRONTIER || cellState == NodeState::PATH)
                    {
                        engine.getGrid().setNodeState(gridPos, NodeState::WALL);
                    }
                }
            }

            // Right-click erase walls
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                NodeState cellState = engine.getGrid().getNodeState(gridPos);
                if (cellState == NodeState::WALL)
                {
                    engine.getGrid().setNodeState(gridPos, NodeState::EMPTY);
                }
            }
        }

        // Release drag
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            draggingStart = false;
            draggingTarget = false;
        }

        // --- UPDATE ---
        engine.update();

        // --- DRAW ---
        BeginDrawing();
        ClearBackground(BG_COLOR);

        // Title
        DrawText("Pathfinding Visualizer", GRID_OFFSET_X, 15, 28, TITLE_COLOR);

        // Subtitle with algorithm info
        const char *algoNames[] = {"BFS", "DFS", "Dijkstra", "A*"};
        int algoIdx = static_cast<int>(engine.getSelectedAlgorithm());
        char subtitle[64];
        snprintf(subtitle, sizeof(subtitle), "Algorithm: %s", algoNames[algoIdx]);
        int subtitleW = MeasureText(subtitle, 16);
        DrawText(subtitle, GRID_OFFSET_X + renderer.gridPixelWidth(engine.getGrid()) - subtitleW,
                 20, 16, SUBTITLE_COLOR);

        // Grid
        renderer.draw(engine.getGrid());

        // UI Panel
        ui.draw(engine);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}