#include "UIManager.hpp"
#include <cstring>

// White theme palette
static const Color PANEL_BG     = {248, 248, 250, 255};
static const Color BTN_NORMAL   = WHITE;
static const Color BTN_HOVER    = {240, 242, 248, 255};
static const Color BTN_ACTIVE   = {66, 133, 244, 255};   // muted blue accent
static const Color BTN_DISABLED = {245, 245, 247, 255};
static const Color BTN_BORDER   = {210, 210, 218, 255};
static const Color TEXT_DARK    = {50, 50, 55, 255};
static const Color TEXT_DIM     = {140, 140, 150, 255};
static const Color SEPARATOR    = {225, 225, 230, 255};

UIManager::UIManager(int panelX, int panelY, int panelWidth, int panelHeight)
    : panelX(panelX), panelY(panelY), panelWidth(panelWidth), panelHeight(panelHeight)
{
}

bool UIManager::drawButton(const Button &btn) const
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btn.rect);

    Color bg = btn.bgColor;
    if (!btn.enabled)
        bg = BTN_DISABLED;
    else if (hovered)
        bg = BTN_HOVER;

    // Simple rectangular button with subtle border
    DrawRectangleRec(btn.rect, bg);
    DrawRectangleLinesEx(btn.rect, 1.0f, btn.enabled ? BTN_BORDER : Color{230, 230, 235, 255});

    // Text centered
    int fontSize = 14;
    int tw = MeasureText(btn.label, fontSize);
    int tx = static_cast<int>(btn.rect.x) + (static_cast<int>(btn.rect.width) - tw) / 2;
    int ty = static_cast<int>(btn.rect.y) + (static_cast<int>(btn.rect.height) - fontSize) / 2;
    DrawText(btn.label, tx, ty, fontSize, btn.enabled ? btn.textColor : TEXT_DIM);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && btn.enabled;
}

bool UIManager::isButtonClicked(const Button &btn) const
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btn.rect);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && btn.enabled;
}

void UIManager::draw(const VisualizerEngine &engine) const
{
    // Panel background
    DrawRectangle(panelX, panelY, panelWidth, panelHeight, PANEL_BG);
    // Left separator
    DrawRectangle(panelX, panelY, 1, panelHeight, SEPARATOR);

    int x = panelX + 16;
    int y = panelY + 16;
    int btnW = panelWidth - 32;
    int btnH = 32;
    int gap = 6;

    auto algoType = engine.getSelectedAlgorithm();
    auto engineState = engine.getState();
    bool canSwitch = (engineState == EngineState::IDLE || engineState == EngineState::FINISHED);
    bool isIdle = canSwitch;
    bool isRunning = (engineState == EngineState::RUNNING);
    bool isPaused = (engineState == EngineState::PAUSED);

    // Section: Algorithms
    DrawText("Algorithm", x, y, 13, TEXT_DIM);
    y += 22;

    const char *algoNames[] = {"BFS", "DFS", "Dijkstra", "A*"};
    AlgorithmType algoTypes[] = {AlgorithmType::BFS, AlgorithmType::DFS,
                                  AlgorithmType::DIJKSTRA, AlgorithmType::ASTAR};

    // 2x2 grid for algorithm buttons
    int halfW = (btnW - gap) / 2;
    for (int i = 0; i < 4; i++)
    {
        bool isSelected = (algoType == algoTypes[i]);
        Color bg = isSelected ? BTN_ACTIVE : BTN_NORMAL;
        Color tc = isSelected ? WHITE : TEXT_DARK;
        int bx = (i % 2 == 0) ? x : x + halfW + gap;
        int by = y;

        Button btn{{static_cast<float>(bx), static_cast<float>(by),
                     static_cast<float>(halfW), static_cast<float>(btnH)},
                    algoNames[i], bg, tc, canSwitch};
        drawButton(btn);

        if (i % 2 == 1)
            y += btnH + gap;
    }
    y += 8;

    // Separator
    DrawRectangle(x, y, btnW, 1, SEPARATOR);
    y += 12;

    // Section: Controls
    DrawText("Controls", x, y, 13, TEXT_DIM);
    y += 22;

    // Start / Resume
    if (isPaused)
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Resume", Color{76, 175, 80, 255}, WHITE, true};
        drawButton(btn);
    }
    else
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Start", Color{76, 175, 80, 255}, WHITE, isIdle};
        drawButton(btn);
    }
    y += btnH + gap;

    // Pause
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Pause", BTN_NORMAL, TEXT_DARK, isRunning};
        drawButton(btn);
    }
    y += btnH + gap;

    // Clear Path
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Clear Path", BTN_NORMAL, TEXT_DARK, true};
        drawButton(btn);
    }
    y += btnH + gap;

    // Reset Grid
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Reset Grid", BTN_NORMAL, TEXT_DARK, true};
        drawButton(btn);
    }
    y += btnH + gap;

    // Generate Maze
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Generate Maze", BTN_ACTIVE, WHITE, isIdle};
        drawButton(btn);
    }
    y += btnH + 8;

    // Separator
    DrawRectangle(x, y, btnW, 1, SEPARATOR);
    y += 12;

    // Section: Speed
    DrawText("Speed", x, y, 13, TEXT_DIM);
    y += 22;

    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(halfW), static_cast<float>(btnH)},
                    "- Slower", BTN_NORMAL, TEXT_DARK, engine.getStepsPerFrame() > 1};
        drawButton(btn);
    }
    {
        Button btn{{static_cast<float>(x + halfW + gap), static_cast<float>(y),
                     static_cast<float>(halfW), static_cast<float>(btnH)},
                    "+ Faster", BTN_NORMAL, TEXT_DARK, engine.getStepsPerFrame() < 50};
        drawButton(btn);
    }
    y += btnH + 4;

    char speedText[32];
    snprintf(speedText, sizeof(speedText), "%d steps/frame", engine.getStepsPerFrame());
    int stw = MeasureText(speedText, 12);
    DrawText(speedText, x + (btnW - stw) / 2, y, 12, TEXT_DIM);
    y += 24;

    // Separator
    DrawRectangle(x, y, btnW, 1, SEPARATOR);
    y += 12;

    // Status
    DrawText("Status", x, y, 13, TEXT_DIM);
    y += 20;
    DrawText(engine.getStatusText().c_str(), x, y, 13, TEXT_DARK);
    y += 28;

    // Help
    DrawText("Left-click: place wall", x, y, 11, TEXT_DIM);
    y += 15;
    DrawText("Right-click: erase wall", x, y, 11, TEXT_DIM);
    y += 15;
    DrawText("Drag S/T to move", x, y, 11, TEXT_DIM);
}

void UIManager::handleInput(VisualizerEngine &engine)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    Vector2 mouse = GetMousePosition();

    int x = panelX + 16;
    int btnW = panelWidth - 32;
    int btnH = 32;
    int gap = 6;
    int halfW = (btnW - gap) / 2;
    int y = panelY + 38; // after "Algorithm" label

    auto engineState = engine.getState();
    bool canSwitch = (engineState == EngineState::IDLE || engineState == EngineState::FINISHED);

    // Algorithm buttons (2x2 grid)
    AlgorithmType algoTypes[] = {AlgorithmType::BFS, AlgorithmType::DFS,
                                  AlgorithmType::DIJKSTRA, AlgorithmType::ASTAR};
    for (int i = 0; i < 4; i++)
    {
        int bx = (i % 2 == 0) ? x : x + halfW + gap;
        int by = y;

        Rectangle rect = {static_cast<float>(bx), static_cast<float>(by),
                           static_cast<float>(halfW), static_cast<float>(btnH)};
        if (canSwitch && CheckCollisionPointRec(mouse, rect))
        {
            engine.selectAlgorithm(algoTypes[i]);
            return;
        }
        if (i % 2 == 1)
            y += btnH + gap;
    }

    // Skip separator + "Controls" label
    y += 8 + 1 + 12 + 22;

    // Start / Resume
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (CheckCollisionPointRec(mouse, rect))
        {
            if (engineState == EngineState::PAUSED)
                engine.resumeVisualization();
            else if (canSwitch)
                engine.startVisualization();
            return;
        }
    }
    y += btnH + gap;

    // Pause
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (engineState == EngineState::RUNNING && CheckCollisionPointRec(mouse, rect))
        {
            engine.pauseVisualization();
            return;
        }
    }
    y += btnH + gap;

    // Clear Path
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (CheckCollisionPointRec(mouse, rect))
        {
            engine.clearPath();
            return;
        }
    }
    y += btnH + gap;

    // Reset Grid
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (CheckCollisionPointRec(mouse, rect))
        {
            engine.resetGrid();
            return;
        }
    }
    y += btnH + gap;

    // Generate Maze
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (canSwitch && CheckCollisionPointRec(mouse, rect))
        {
            engine.generateMaze();
            return;
        }
    }
    y += btnH + 8;

    // Skip separator + "Speed" label
    y += 1 + 12 + 22;

    // Speed buttons
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(halfW), static_cast<float>(btnH)};
        if (CheckCollisionPointRec(mouse, rect))
        {
            int spf = engine.getStepsPerFrame();
            if (spf > 1)
                engine.setStepsPerFrame(spf - 1);
            return;
        }
    }
    {
        Rectangle rect = {static_cast<float>(x + halfW + gap), static_cast<float>(y),
                           static_cast<float>(halfW), static_cast<float>(btnH)};
        if (CheckCollisionPointRec(mouse, rect))
        {
            int spf = engine.getStepsPerFrame();
            if (spf < 50)
                engine.setStepsPerFrame(spf + 1);
            return;
        }
    }
}
