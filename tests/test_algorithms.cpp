#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "GridModel.hpp"
#include "BFS.hpp"
#include "DFS.hpp"
#include "Dijkstra.hpp"
#include "AStar.hpp"
#include "MazeGenerator.hpp"
#include <algorithm>
#include <cmath>

// Helper: run an algorithm to completion and return the path
static std::vector<Position> runToCompletion(IPathfinder &algo, GridModel &model,
                                              Position start, Position target)
{
    algo.initialize(model, start, target);
    int maxSteps = model.getRows() * model.getCols() * 4; // safety limit
    int steps = 0;
    while (!algo.isComplete() && steps < maxSteps)
    {
        algo.step(model);
        steps++;
    }
    return algo.getPath();
}

// Helper: verify path is contiguous (each step is 4-directional neighbor)
static bool isContiguousPath(const std::vector<Position> &path)
{
    for (size_t i = 1; i < path.size(); i++)
    {
        int dr = std::abs(path[i].row - path[i - 1].row);
        int dc = std::abs(path[i].col - path[i - 1].col);
        if (dr + dc != 1)
            return false;
    }
    return true;
}

// Helper: Manhattan distance
static int manhattan(Position a, Position b)
{
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}

// ===== GRID MODEL TESTS =====

TEST_CASE("GridModel: construction and bounds")
{
    GridModel grid(10, 15);
    CHECK(grid.getRows() == 10);
    CHECK(grid.getCols() == 15);
    CHECK(grid.inBounds({0, 0}));
    CHECK(grid.inBounds({9, 14}));
    CHECK_FALSE(grid.inBounds({-1, 0}));
    CHECK_FALSE(grid.inBounds({0, -1}));
    CHECK_FALSE(grid.inBounds({10, 0}));
    CHECK_FALSE(grid.inBounds({0, 15}));
}

TEST_CASE("GridModel: node state manipulation")
{
    GridModel grid(5, 5);
    CHECK(grid.getNodeState({2, 2}) == NodeState::EMPTY);

    grid.setNodeState({2, 2}, NodeState::WALL);
    CHECK(grid.getNodeState({2, 2}) == NodeState::WALL);
    CHECK(grid.isWall({2, 2}));

    grid.setNodeState({2, 2}, NodeState::START);
    CHECK(grid.getNodeState({2, 2}) == NodeState::START);
    CHECK_FALSE(grid.isWall({2, 2}));
}

TEST_CASE("GridModel: resetStates preserves walls and endpoints")
{
    GridModel grid(5, 5);
    grid.setNodeState({0, 0}, NodeState::START);
    grid.setNodeState({4, 4}, NodeState::TARGET);
    grid.setNodeState({1, 1}, NodeState::WALL);
    grid.setNodeState({2, 2}, NodeState::VISITED);
    grid.setNodeState({3, 3}, NodeState::FRONTIER);

    grid.resetStates();

    CHECK(grid.getNodeState({0, 0}) == NodeState::START);
    CHECK(grid.getNodeState({4, 4}) == NodeState::TARGET);
    CHECK(grid.getNodeState({1, 1}) == NodeState::WALL);
    CHECK(grid.getNodeState({2, 2}) == NodeState::EMPTY);
    CHECK(grid.getNodeState({3, 3}) == NodeState::EMPTY);
}

// ===== ALGORITHM TEST MACROS =====
// Each algorithm is tested with the same scenarios via SUBCASE

#define ALGO_TEST_SUITE(AlgoName, AlgoClass, CheckShortest)                                      \
    TEST_CASE("AlgoName: direct path on empty grid")                                             \
    {                                                                                            \
        GridModel grid(10, 10);                                                                  \
        Position start{0, 0};                                                                    \
        Position target{0, 5};                                                                   \
        grid.setNodeState(start, NodeState::START);                                              \
        grid.setNodeState(target, NodeState::TARGET);                                            \
                                                                                                 \
        AlgoClass algo;                                                                          \
        auto path = runToCompletion(algo, grid, start, target);                                  \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK(algo.isPathFound());                                                               \
        CHECK(path.front() == start);                                                            \
        CHECK(path.back() == target);                                                            \
        CHECK(isContiguousPath(path));                                                           \
        if (CheckShortest)                                                                       \
        {                                                                                        \
            CHECK(static_cast<int>(path.size()) == manhattan(start, target) + 1);                \
        }                                                                                        \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: path around obstacle")                                                  \
    {                                                                                            \
        GridModel grid(5, 10);                                                                   \
        Position start{2, 0};                                                                    \
        Position target{2, 9};                                                                   \
        grid.setNodeState(start, NodeState::START);                                              \
        grid.setNodeState(target, NodeState::TARGET);                                            \
        /* Wall blocking row 2, cols 3-6 */                                                      \
        for (int c = 3; c <= 6; c++)                                                             \
            grid.setNodeState({2, c}, NodeState::WALL);                                          \
                                                                                                 \
        AlgoClass algo;                                                                          \
        auto path = runToCompletion(algo, grid, start, target);                                  \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK(algo.isPathFound());                                                               \
        CHECK(path.front() == start);                                                            \
        CHECK(path.back() == target);                                                            \
        CHECK(isContiguousPath(path));                                                           \
        /* Verify no wall cells in path */                                                       \
        for (const auto &p : path)                                                               \
        {                                                                                        \
            CHECK_FALSE(grid.isWall(p));                                                         \
        }                                                                                        \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: unreachable target")                                                    \
    {                                                                                            \
        GridModel grid(5, 5);                                                                    \
        Position start{0, 0};                                                                    \
        Position target{4, 4};                                                                   \
        grid.setNodeState(start, NodeState::START);                                              \
        grid.setNodeState(target, NodeState::TARGET);                                            \
        /* Surround target with walls */                                                         \
        grid.setNodeState({3, 4}, NodeState::WALL);                                              \
        grid.setNodeState({4, 3}, NodeState::WALL);                                              \
        grid.setNodeState({3, 3}, NodeState::WALL);                                              \
                                                                                                 \
        AlgoClass algo;                                                                          \
        runToCompletion(algo, grid, start, target);                                              \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK_FALSE(algo.isPathFound());                                                         \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: start equals target")                                                   \
    {                                                                                            \
        GridModel grid(5, 5);                                                                    \
        Position pos{2, 2};                                                                      \
        grid.setNodeState(pos, NodeState::START);                                                \
                                                                                                 \
        AlgoClass algo;                                                                          \
        auto path = runToCompletion(algo, grid, pos, pos);                                       \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK(algo.isPathFound());                                                               \
        CHECK(path.size() == 1);                                                                 \
        CHECK(path.front() == pos);                                                              \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: start is wall")                                                         \
    {                                                                                            \
        GridModel grid(5, 5);                                                                    \
        Position start{0, 0};                                                                    \
        Position target{4, 4};                                                                   \
        grid.setNodeState(start, NodeState::WALL);                                               \
        grid.setNodeState(target, NodeState::TARGET);                                            \
                                                                                                 \
        AlgoClass algo;                                                                          \
        runToCompletion(algo, grid, start, target);                                              \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK_FALSE(algo.isPathFound());                                                         \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: out-of-bounds start")                                                   \
    {                                                                                            \
        GridModel grid(5, 5);                                                                    \
        Position start{-1, 0};                                                                   \
        Position target{4, 4};                                                                   \
        grid.setNodeState(target, NodeState::TARGET);                                            \
                                                                                                 \
        AlgoClass algo;                                                                          \
        runToCompletion(algo, grid, start, target);                                              \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK_FALSE(algo.isPathFound());                                                         \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: single cell grid")                                                      \
    {                                                                                            \
        GridModel grid(1, 1);                                                                    \
        Position pos{0, 0};                                                                      \
        grid.setNodeState(pos, NodeState::START);                                                \
                                                                                                 \
        AlgoClass algo;                                                                          \
        auto path = runToCompletion(algo, grid, pos, pos);                                       \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK(algo.isPathFound());                                                               \
        CHECK(path.size() == 1);                                                                 \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: boundary path")                                                         \
    {                                                                                            \
        GridModel grid(3, 3);                                                                    \
        Position start{0, 0};                                                                    \
        Position target{2, 2};                                                                   \
        grid.setNodeState(start, NodeState::START);                                              \
        grid.setNodeState(target, NodeState::TARGET);                                            \
        /* Block center */                                                                       \
        grid.setNodeState({1, 1}, NodeState::WALL);                                              \
                                                                                                 \
        AlgoClass algo;                                                                          \
        auto path = runToCompletion(algo, grid, start, target);                                  \
                                                                                                 \
        CHECK(algo.isComplete());                                                                \
        CHECK(algo.isPathFound());                                                               \
        CHECK(path.front() == start);                                                            \
        CHECK(path.back() == target);                                                            \
        CHECK(isContiguousPath(path));                                                           \
    }                                                                                            \
                                                                                                 \
    TEST_CASE("AlgoName: repeated runs")                                                         \
    {                                                                                            \
        GridModel grid(5, 5);                                                                    \
        Position start{0, 0};                                                                    \
        Position target{4, 4};                                                                   \
        grid.setNodeState(start, NodeState::START);                                              \
        grid.setNodeState(target, NodeState::TARGET);                                            \
                                                                                                 \
        AlgoClass algo;                                                                          \
        auto path1 = runToCompletion(algo, grid, start, target);                                 \
        CHECK(algo.isPathFound());                                                               \
                                                                                                 \
        /* Reset and run again */                                                                \
        grid.resetStates();                                                                      \
        grid.setNodeState(start, NodeState::START);                                              \
        grid.setNodeState(target, NodeState::TARGET);                                            \
        auto path2 = runToCompletion(algo, grid, start, target);                                 \
        CHECK(algo.isPathFound());                                                               \
        CHECK(isContiguousPath(path2));                                                          \
    }

// Instantiate test suites for each algorithm
// BFS: must find shortest path (CheckShortest = true)
ALGO_TEST_SUITE(BFS, BFS, true)

// DFS: does NOT guarantee shortest path (CheckShortest = false)
ALGO_TEST_SUITE(DFS, DFS, false)

// Dijkstra: must find shortest path (CheckShortest = true)
ALGO_TEST_SUITE(Dijkstra, Dijkstra, true)

// A*: must find shortest path (CheckShortest = true)
ALGO_TEST_SUITE(AStar, AStar, true)

// ===== MAZE GENERATOR TESTS =====

TEST_CASE("MazeGenerator: generates walls and open passages")
{
    GridModel grid(25, 40);
    Position start{12, 10};
    Position target{12, 30};

    MazeGenerator::generate(grid, start, target, 42);

    CHECK(grid.getNodeState(start) == NodeState::START);
    CHECK(grid.getNodeState(target) == NodeState::TARGET);

    int wallCount = 0;
    int emptyCount = 0;
    for (int r = 0; r < grid.getRows(); r++)
    {
        for (int c = 0; c < grid.getCols(); c++)
        {
            if (grid.getNodeState({r, c}) == NodeState::WALL)
                wallCount++;
            else if (grid.getNodeState({r, c}) == NodeState::EMPTY)
                emptyCount++;
        }
    }

    // A valid maze must have a substantial number of both walls and passages
    CHECK(wallCount > 100);
    CHECK(emptyCount > 100);
}

TEST_CASE("MazeGenerator: start and target accessibility with all 4 algorithms")
{
    // Test on default visualizer grid size (25x40) with even coordinates
    GridModel grid(25, 40);
    Position start{12, 10};
    Position target{12, 30};

    MazeGenerator::generate(grid, start, target, 12345);

    // BFS
    {
        BFS bfs;
        auto path = runToCompletion(bfs, grid, start, target);
        CHECK(bfs.isPathFound());
        CHECK(isContiguousPath(path));
        CHECK(path.front() == start);
        CHECK(path.back() == target);
    }

    // DFS
    {
        grid.resetStates();
        grid.setNodeState(start, NodeState::START);
        grid.setNodeState(target, NodeState::TARGET);
        DFS dfs;
        auto path = runToCompletion(dfs, grid, start, target);
        CHECK(dfs.isPathFound());
        CHECK(isContiguousPath(path));
        CHECK(path.front() == start);
        CHECK(path.back() == target);
    }

    // Dijkstra
    {
        grid.resetStates();
        grid.setNodeState(start, NodeState::START);
        grid.setNodeState(target, NodeState::TARGET);
        Dijkstra dijkstra;
        auto path = runToCompletion(dijkstra, grid, start, target);
        CHECK(dijkstra.isPathFound());
        CHECK(isContiguousPath(path));
        CHECK(path.front() == start);
        CHECK(path.back() == target);
    }

    // A*
    {
        grid.resetStates();
        grid.setNodeState(start, NodeState::START);
        grid.setNodeState(target, NodeState::TARGET);
        AStar astar;
        auto path = runToCompletion(astar, grid, start, target);
        CHECK(astar.isPathFound());
        CHECK(isContiguousPath(path));
        CHECK(path.front() == start);
        CHECK(path.back() == target);
    }
}

TEST_CASE("MazeGenerator: corner positions are solvable")
{
    GridModel grid(15, 15);
    Position start{0, 0};
    Position target{14, 14};

    MazeGenerator::generate(grid, start, target, 999);

    BFS bfs;
    auto path = runToCompletion(bfs, grid, start, target);
    CHECK(bfs.isPathFound());
    CHECK(isContiguousPath(path));
}

TEST_CASE("MazeGenerator: deterministic seeded generation")
{
    GridModel grid1(21, 21);
    GridModel grid2(21, 21);
    Position start{5, 5};
    Position target{15, 15};

    MazeGenerator::generate(grid1, start, target, 777);
    MazeGenerator::generate(grid2, start, target, 777);

    for (int r = 0; r < 21; r++)
    {
        for (int c = 0; c < 21; c++)
        {
            CHECK(grid1.getNodeState({r, c}) == grid2.getNodeState({r, c}));
        }
    }

    // Different seeds produce different mazes
    GridModel grid3(21, 21);
    MazeGenerator::generate(grid3, start, target, 888);

    bool anyDifference = false;
    for (int r = 0; r < 21; r++)
    {
        for (int c = 0; c < 21; c++)
        {
            if (grid1.getNodeState({r, c}) != grid3.getNodeState({r, c}))
            {
                anyDifference = true;
                break;
            }
        }
        if (anyDifference) break;
    }
    CHECK(anyDifference);
}

