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

#include <string>
#include <cstring>
static Font loadAppFont()
{
    const char *candidates[] = {
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
    };

    for (const char *path : candidates)
    {
        if (FileExists(path))
        {
            Font f = LoadFontEx(path, 36, nullptr, 0);
            if (f.texture.id > 0)
            {
                SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
                return f;
            }
        }
    }
    return GetFontDefault();
}

int main(int argc, char *argv[])
{
    bool screenshotMode = false;
    std::string screenshotPath;
    bool mazeMode = false;
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--screenshot") == 0 && i + 1 < argc)
        {
            screenshotMode = true;
            screenshotPath = argv[++i];
        }
        else if (std::strcmp(argv[i], "--maze") == 0)
        {
            mazeMode = true;
        }
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pathfinding Visualizer");
    SetTargetFPS(60);

    Font appFont = loadAppFont();

    VisualizerEngine engine(GRID_ROWS, GRID_COLS);
    if (mazeMode)
    {
        engine.generateMaze();
    }
    GridRenderer renderer(GRID_OFFSET_X, GRID_OFFSET_Y, CELL_SIZE, appFont);
    int panelX = GRID_OFFSET_X + GRID_COLS * CELL_SIZE + 10;
    UIManager ui(panelX, 0, SCREEN_WIDTH - panelX, SCREEN_HEIGHT, appFont);

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

        // Title (clean, plain, minimal font with neutral appearance)
        DrawTextEx(appFont, "Pathfinding Visualizer",
                   Vector2{static_cast<float>(GRID_OFFSET_X), 18.0f},
                   20.0f, 0.0f, TITLE_COLOR);

        // Subtitle with algorithm info
        const char *algoNames[] = {"BFS", "DFS", "Dijkstra", "A*"};
        int algoIdx = static_cast<int>(engine.getSelectedAlgorithm());
        char subtitle[64];
        snprintf(subtitle, sizeof(subtitle), "Algorithm: %s", algoNames[algoIdx]);
        float subtitleW = MeasureTextEx(appFont, subtitle, 14.0f, 0.0f).x;
        DrawTextEx(appFont, subtitle,
                   Vector2{static_cast<float>(GRID_OFFSET_X + renderer.gridPixelWidth(engine.getGrid())) - subtitleW, 22.0f},
                   14.0f, 0.0f, SUBTITLE_COLOR);

        // Grid
        renderer.draw(engine.getGrid());

        // UI Panel
        ui.draw(engine);

        EndDrawing();
        if (screenshotMode)
        {
            static int frameCount = 0;
            if (++frameCount >= 2)
            {
                TakeScreenshot(screenshotPath.c_str());
                break;
            }
        }
    }

    if (appFont.texture.id != GetFontDefault().texture.id)
    {
        UnloadFont(appFont);
    }

    CloseWindow();
    return 0;
}