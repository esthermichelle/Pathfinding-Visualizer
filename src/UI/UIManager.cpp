#include "UIManager.hpp"
#include <cstring>

static const Color BG_DARK      = {30, 30, 46, 255};
static const Color PANEL_BG     = {36, 36, 54, 255};
static const Color BTN_NORMAL   = {55, 55, 80, 255};
static const Color BTN_HOVER    = {75, 75, 110, 255};
static const Color BTN_ACTIVE   = {0, 212, 170, 255};
static const Color BTN_DISABLED = {40, 40, 55, 255};
static const Color TEXT_WHITE   = {230, 230, 245, 255};
static const Color TEXT_DIM     = {140, 140, 170, 255};
static const Color ACCENT_GREEN = {0, 212, 170, 255};
static const Color ACCENT_RED   = {255, 107, 107, 255};
static const Color ACCENT_BLUE  = {116, 185, 255, 255};
static const Color ACCENT_GOLD  = {255, 217, 61, 255};

UIManager::UIManager(int panelX, int panelY, int panelWidth, int panelHeight)
    : panelX(panelX), panelY(panelY), panelWidth(panelWidth), panelHeight(panelHeight)
{
}

bool UIManager::drawButton(const Button &btn) const
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btn.rect);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && btn.enabled;

    Color bg = btn.bgColor;
    if (!btn.enabled)
        bg = BTN_DISABLED;
    else if (hovered)
        bg = BTN_HOVER;

    // Rounded button
    DrawRectangleRounded(btn.rect, 0.3f, 6, bg);

    // Text centered
    int fontSize = 16;
    int tw = MeasureText(btn.label, fontSize);
    int tx = static_cast<int>(btn.rect.x) + (static_cast<int>(btn.rect.width) - tw) / 2;
    int ty = static_cast<int>(btn.rect.y) + (static_cast<int>(btn.rect.height) - fontSize) / 2;
    DrawText(btn.label, tx, ty, fontSize, btn.enabled ? btn.textColor : TEXT_DIM);

    return clicked;
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
    // Separator line
    DrawRectangle(panelX, panelY, 2, panelHeight, Color{60, 60, 90, 255});

    int x = panelX + 20;
    int y = panelY + 20;
    int btnW = panelWidth - 40;
    int btnH = 36;
    int gap = 8;

    // Title
    DrawText("ALGORITHMS", x, y, 18, ACCENT_GREEN);
    y += 30;

    // Algorithm buttons
    auto algoType = engine.getSelectedAlgorithm();
    auto engineState = engine.getState();
    bool canSwitch = (engineState == EngineState::IDLE || engineState == EngineState::FINISHED);

    const char *algoNames[] = {"BFS", "DFS", "Dijkstra", "A*"};
    AlgorithmType algoTypes[] = {AlgorithmType::BFS, AlgorithmType::DFS,
                                  AlgorithmType::DIJKSTRA, AlgorithmType::ASTAR};

    for (int i = 0; i < 4; i++)
    {
        bool isSelected = (algoType == algoTypes[i]);
        Color bg = isSelected ? BTN_ACTIVE : BTN_NORMAL;
        Color tc = isSelected ? Color{0, 0, 0, 255} : TEXT_WHITE;

        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    algoNames[i], bg, tc, canSwitch};
        drawButton(btn);
        y += btnH + gap;
    }

    // Separator
    y += 10;
    DrawRectangle(x, y, btnW, 1, Color{60, 60, 90, 255});
    y += 20;

    // Control section
    DrawText("CONTROLS", x, y, 18, ACCENT_BLUE);
    y += 30;

    bool isIdle = (engineState == EngineState::IDLE || engineState == EngineState::FINISHED);
    bool isRunning = (engineState == EngineState::RUNNING);
    bool isPaused = (engineState == EngineState::PAUSED);

    // Start / Resume button
    if (isPaused)
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Resume", ACCENT_GREEN, Color{0, 0, 0, 255}, true};
        drawButton(btn);
    }
    else
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Start", ACCENT_GREEN, Color{0, 0, 0, 255}, isIdle};
        drawButton(btn);
    }
    y += btnH + gap;

    // Pause button
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Pause", ACCENT_GOLD, Color{0, 0, 0, 255}, isRunning};
        drawButton(btn);
    }
    y += btnH + gap;

    // Clear Path button
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Clear Path", BTN_NORMAL, TEXT_WHITE, !isIdle || engineState == EngineState::FINISHED};
        drawButton(btn);
    }
    y += btnH + gap;

    // Reset Grid button
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(btnW), static_cast<float>(btnH)},
                    "Reset Grid", ACCENT_RED, TEXT_WHITE, true};
        drawButton(btn);
    }
    y += btnH + gap;

    // Separator
    y += 10;
    DrawRectangle(x, y, btnW, 1, Color{60, 60, 90, 255});
    y += 20;

    // Speed controls
    DrawText("SPEED", x, y, 18, ACCENT_GOLD);
    y += 30;

    int halfW = (btnW - gap) / 2;
    {
        Button btn{{static_cast<float>(x), static_cast<float>(y),
                     static_cast<float>(halfW), static_cast<float>(btnH)},
                    "Slower", BTN_NORMAL, TEXT_WHITE, engine.getStepsPerFrame() > 1};
        drawButton(btn);
    }
    {
        Button btn{{static_cast<float>(x + halfW + gap), static_cast<float>(y),
                     static_cast<float>(halfW), static_cast<float>(btnH)},
                    "Faster", BTN_NORMAL, TEXT_WHITE, engine.getStepsPerFrame() < 50};
        drawButton(btn);
    }
    y += btnH + 5;

    char speedText[32];
    snprintf(speedText, sizeof(speedText), "Steps/frame: %d", engine.getStepsPerFrame());
    DrawText(speedText, x, y, 14, TEXT_DIM);
    y += 30;

    // Separator
    DrawRectangle(x, y, btnW, 1, Color{60, 60, 90, 255});
    y += 20;

    // Status
    DrawText("STATUS", x, y, 18, TEXT_DIM);
    y += 25;
    DrawText(engine.getStatusText().c_str(), x, y, 14, TEXT_WHITE);
    y += 30;

    // Instructions
    DrawText("CONTROLS", x, y, 14, TEXT_DIM);
    y += 20;
    DrawText("Left-click: Wall", x, y, 12, TEXT_DIM);
    y += 16;
    DrawText("Right-click: Erase", x, y, 12, TEXT_DIM);
    y += 16;
    DrawText("Drag S/T: Move", x, y, 12, TEXT_DIM);
}

void UIManager::handleInput(VisualizerEngine &engine)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    Vector2 mouse = GetMousePosition();

    int x = panelX + 20;
    int btnW = panelWidth - 40;
    int btnH = 36;
    int gap = 8;
    int y = panelY + 50; // after title

    auto engineState = engine.getState();
    bool canSwitch = (engineState == EngineState::IDLE || engineState == EngineState::FINISHED);

    // Algorithm buttons
    AlgorithmType algoTypes[] = {AlgorithmType::BFS, AlgorithmType::DFS,
                                  AlgorithmType::DIJKSTRA, AlgorithmType::ASTAR};
    for (int i = 0; i < 4; i++)
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (canSwitch && CheckCollisionPointRec(mouse, rect))
        {
            engine.selectAlgorithm(algoTypes[i]);
            return;
        }
        y += btnH + gap;
    }

    // Skip separator
    y += 10 + 1 + 20 + 30;

    // Start / Resume
    {
        Rectangle rect = {static_cast<float>(x), static_cast<float>(y),
                           static_cast<float>(btnW), static_cast<float>(btnH)};
        if (CheckCollisionPointRec(mouse, rect))
        {
            if (engineState == EngineState::PAUSED)
                engine.resumeVisualization();
            else if (engineState == EngineState::IDLE || engineState == EngineState::FINISHED)
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

    // Skip separator
    y += 10 + 1 + 20 + 30;

    // Speed buttons
    int halfW = (btnW - gap) / 2;
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
