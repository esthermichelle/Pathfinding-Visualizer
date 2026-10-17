#pragma once

#include "GridModel.hpp"
#include "IPathfinder.hpp"
#include <memory>
#include <string>

enum class AlgorithmType
{
    BFS,
    DFS,
    DIJKSTRA,
    ASTAR
};

enum class EngineState
{
    IDLE,       // No algorithm running
    RUNNING,    // Algorithm stepping automatically
    PAUSED,     // Algorithm paused mid-run
    FINISHED    // Algorithm completed
};

class VisualizerEngine
{
public:
    VisualizerEngine(int rows, int cols);

    // Algorithm control
    void selectAlgorithm(AlgorithmType type);
    void startVisualization();
    void pauseVisualization();
    void resumeVisualization();
    void stepOnce();

    // Grid operations
    void clearPath();       // Clear visualization but keep walls
    void resetGrid();       // Clear everything

    // Endpoint manipulation (for dragging)
    void moveStartTo(Position newPos);
    void moveTargetTo(Position newPos);

    // Per-frame update (steps algorithm if running)
    void update();

    // Accessors
    GridModel &getGrid() { return grid; }
    const GridModel &getGrid() const { return grid; }
    EngineState getState() const { return state; }
    AlgorithmType getSelectedAlgorithm() const { return selectedAlgorithm; }
    const std::string &getStatusText() const { return statusText; }
    Position getStartPos() const { return startPos; }
    Position getTargetPos() const { return targetPos; }
    int getStepsPerFrame() const { return stepsPerFrame; }
    void setStepsPerFrame(int spf);

private:
    GridModel grid;
    std::unique_ptr<IPathfinder> pathfinder;

    AlgorithmType selectedAlgorithm = AlgorithmType::BFS;
    EngineState state = EngineState::IDLE;

    Position startPos;
    Position targetPos;

    // Dragging state
    bool draggingStart = false;
    bool draggingTarget = false;

    int stepsPerFrame = 1;
    int frameCounter = 0;
    int framesPerStep = 2; // step every N frames for visible animation

    std::string statusText = "Select an algorithm and press Start";

    std::unique_ptr<IPathfinder> createPathfinder(AlgorithmType type) const;
    void markPath();
};
