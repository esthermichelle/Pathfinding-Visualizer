#include "VisualizerEngine.hpp"
#include "BFS.hpp"
#include "DFS.hpp"
#include "Dijkstra.hpp"
#include "AStar.hpp"
#include "MazeGenerator.hpp"
#include "raylib.h"

VisualizerEngine::VisualizerEngine(int rows, int cols)
    : grid(rows, cols),
      startPos{rows / 2, cols / 4},
      targetPos{rows / 2, 3 * cols / 4}
{
    grid.setNodeState(startPos, NodeState::START);
    grid.setNodeState(targetPos, NodeState::TARGET);
}

std::unique_ptr<IPathfinder> VisualizerEngine::createPathfinder(AlgorithmType type) const
{
    switch (type)
    {
    case AlgorithmType::BFS:
        return std::make_unique<BFS>();
    case AlgorithmType::DFS:
        return std::make_unique<DFS>();
    case AlgorithmType::DIJKSTRA:
        return std::make_unique<Dijkstra>();
    case AlgorithmType::ASTAR:
        return std::make_unique<AStar>();
    default:
        return std::make_unique<BFS>();
    }
}

void VisualizerEngine::selectAlgorithm(AlgorithmType type)
{
    if (state == EngineState::RUNNING || state == EngineState::PAUSED)
        return; // Don't switch mid-run

    selectedAlgorithm = type;

    switch (type)
    {
    case AlgorithmType::BFS:
        statusText = "BFS selected";
        break;
    case AlgorithmType::DFS:
        statusText = "DFS selected";
        break;
    case AlgorithmType::DIJKSTRA:
        statusText = "Dijkstra selected";
        break;
    case AlgorithmType::ASTAR:
        statusText = "A* selected";
        break;
    }
}

void VisualizerEngine::startVisualization()
{
    if (state == EngineState::RUNNING)
        return;

    // Clear previous visualization state
    clearPath();

    pathfinder = createPathfinder(selectedAlgorithm);
    pathfinder->initialize(grid, startPos, targetPos);

    if (pathfinder->isComplete())
    {
        // Immediate completion (e.g. start==target)
        state = EngineState::FINISHED;
        if (pathfinder->isPathFound())
        {
            markPath();
            statusText = "Path found! (start == target)";
        }
        else
        {
            statusText = "No path possible";
        }
        return;
    }

    state = EngineState::RUNNING;
    frameCounter = 0;
    statusText = "Running...";
}

void VisualizerEngine::pauseVisualization()
{
    if (state == EngineState::RUNNING)
    {
        state = EngineState::PAUSED;
        statusText = "Paused";
    }
}

void VisualizerEngine::resumeVisualization()
{
    if (state == EngineState::PAUSED)
    {
        state = EngineState::RUNNING;
        statusText = "Running...";
    }
}

void VisualizerEngine::stepOnce()
{
    if (!pathfinder || pathfinder->isComplete())
        return;

    bool done = pathfinder->step(grid);
    if (done)
    {
        state = EngineState::FINISHED;
        if (pathfinder->isPathFound())
        {
            markPath();
            auto path = pathfinder->getPath();
            statusText = "Path found! Length: " + std::to_string(path.size());
        }
        else
        {
            statusText = "No path found";
        }
    }
}

void VisualizerEngine::clearPath()
{
    grid.resetStates();
    // Ensure start/target are restored
    grid.setNodeState(startPos, NodeState::START);
    grid.setNodeState(targetPos, NodeState::TARGET);
    pathfinder.reset();
    state = EngineState::IDLE;
    statusText = "Path cleared";
}

void VisualizerEngine::resetGrid()
{
    int rows = grid.getRows();
    int cols = grid.getCols();
    grid = GridModel(rows, cols);
    startPos = {rows / 2, cols / 4};
    targetPos = {rows / 2, 3 * cols / 4};
    grid.setNodeState(startPos, NodeState::START);
    grid.setNodeState(targetPos, NodeState::TARGET);
    pathfinder.reset();
    state = EngineState::IDLE;
    statusText = "Grid reset";
}

void VisualizerEngine::generateMaze()
{
    // Stop any running search
    pathfinder.reset();
    state = EngineState::IDLE;

    MazeGenerator::generate(grid, startPos, targetPos);
    statusText = "Maze generated";
}

void VisualizerEngine::setStepsPerFrame(int spf)
{
    if (spf >= 1 && spf <= 50)
        stepsPerFrame = spf;
}

void VisualizerEngine::moveStartTo(Position newPos)
{
    if (!grid.inBounds(newPos))
        return;
    NodeState destState = grid.getNodeState(newPos);
    if (destState == NodeState::WALL || destState == NodeState::TARGET)
        return;

    grid.setNodeState(startPos, NodeState::EMPTY);
    startPos = newPos;
    grid.setNodeState(startPos, NodeState::START);
}

void VisualizerEngine::moveTargetTo(Position newPos)
{
    if (!grid.inBounds(newPos))
        return;
    NodeState destState = grid.getNodeState(newPos);
    if (destState == NodeState::WALL || destState == NodeState::START)
        return;

    grid.setNodeState(targetPos, NodeState::EMPTY);
    targetPos = newPos;
    grid.setNodeState(targetPos, NodeState::TARGET);
}

void VisualizerEngine::update()
{
    if (state != EngineState::RUNNING || !pathfinder)
        return;

    frameCounter++;
    if (frameCounter < framesPerStep)
        return;

    frameCounter = 0;

    for (int i = 0; i < stepsPerFrame; i++)
    {
        stepOnce();
        if (state == EngineState::FINISHED)
            break;
    }
}

void VisualizerEngine::markPath()
{
    if (!pathfinder || !pathfinder->isPathFound())
        return;

    auto path = pathfinder->getPath();
    for (const auto &pos : path)
    {
        if (pos != startPos && pos != targetPos)
        {
            grid.setNodeState(pos, NodeState::PATH);
        }
    }
}
