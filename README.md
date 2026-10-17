# Pathfinding Visualizer

An interactive 2D pathfinding visualization tool built with **C++17** and **Raylib**, designed with a **clean, minimal, modern white interface**. Watch BFS, DFS, Dijkstra, and A\* explore a grid step-by-step, generate complex procedural mazes, place walls, drag endpoints, and compare algorithms in real time.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Raylib](https://img.shields.io/badge/Raylib-5.5-green)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-red)

## Features

- **Four pathfinding algorithms**: BFS, DFS, Dijkstra, A\* (Manhattan heuristic)
- **Procedural Maze Generator**: Instant randomized DFS (recursive backtracker) mazes with guaranteed solvability
- **Minimal White Theme**: Clean white background, subtle light-gray grid lines, dark charcoal typography, and restrained blue accents
- **Step-by-step animation**: Non-blocking incremental visualization at configurable speed (1–50 steps/frame)
- **Interactive grid**: Left-click to place walls, right-click to erase
- **Draggable endpoints**: Click and drag Start (S) or Target (T) anywhere on the grid
- **Full simulation controls**: Start, Pause, Resume, Clear Path, Reset Grid, Generate Maze
- **Comprehensive test suite**: 43 headless tests (657 assertions) covering correctness, edge cases, shortest-path guarantees, and maze solvability

## Prerequisites

| Tool | Minimum Version | Notes |
|------|----------------|-------|
| C++ compiler | C++17 support | GCC 8+, Clang 7+, MSVC 2017+ |
| CMake | 3.16+ | Build system |
| Git | Any | Raylib is fetched via FetchContent |

**No manual Raylib installation needed** — CMake downloads and builds it automatically via `FetchContent`.

### Windows (MSYS2/UCRT64)

```bash
# Install prerequisites via MSYS2
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make
```

## Build Instructions

### Configure and Build

```bash
# Configure (first time — fetches Raylib, takes a few minutes)
cmake -S . -B build -G "MinGW Makefiles" \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++

# Build
cmake --build build

# Run the visualizer
./build/PathfindingVisualizer.exe
```

### Run Tests

```bash
# Build and run tests
cmake --build build
cd build && ctest --output-on-failure

# Or run the test executable directly for detailed output
./build/PathfindingTests.exe
```

## Controls

| Action | Input |
|--------|-------|
| Place wall | Left-click on empty cell |
| Remove wall | Right-click on wall |
| Drag Start/Target | Left-click and drag S or T |
| Select algorithm | Click algorithm button in sidebar (BFS, DFS, Dijkstra, A\*) |
| Start visualization | Click **Start** button |
| Pause / Resume | Click **Pause** / **Resume** button |
| Clear path | Click **Clear Path** (clears search, keeps walls) |
| Reset grid | Click **Reset Grid** (clears everything to empty grid) |
| Generate maze | Click **Generate Maze** (creates randomized DFS maze) |
| Adjust speed | Click **- Slower** / **+ Faster** buttons |
| Quit | Close window or press Escape |

## Algorithms

| Algorithm | Optimal? | Data Structure | Description |
|-----------|----------|---------------|-------------|
| **BFS** | ✅ Yes | Queue | Explores level-by-level; guarantees shortest path on unweighted grids |
| **DFS** | ❌ No | Stack | Explores depth-first; finds *a* path but not necessarily the shortest |
| **Dijkstra** | ✅ Yes | Priority Queue | Finds shortest path using cumulative cost; handles stale PQ entries |
| **A\*** | ✅ Yes | Priority Queue | Dijkstra + Manhattan heuristic; optimal and typically explores fewer nodes |

All algorithms operate with **4-directional movement** (up, down, left, right) with **uniform step weights** (1.0).

## Maze Generation

The built-in maze generator employs an **iterative randomized Depth-First Search (recursive backtracker)**:
- Carves corridors across a subgrid of odd-coordinate cells to create distinct single-cell passages and walls.
- Uses an explicit `std::stack` to avoid call-stack overflow on large grids.
- Automatically connects Start and Target nodes to the carved passage network, guaranteeing 100% path solvability between Start and Target regardless of their grid positions.
- Seeded generation support enables deterministic and reproducible mazes in automated unit tests.

## Architecture

```
include/
├── Core/
│   └── Types.hpp            # NodeState enum, Position struct
├── Grid/
│   ├── Node.hpp             # Node with pos, state, parent, costs
│   └── GridModel.hpp        # Graphics-independent grid data model
├── Algorithms/
│   ├── IPathfinder.hpp      # Abstract interface for all algorithms
│   ├── BFS.hpp
│   ├── DFS.hpp
│   ├── Dijkstra.hpp
│   └── AStar.hpp
├── Maze/
│   └── MazeGenerator.hpp    # Procedural maze generator (DFS backtracker)
├── Rendering/
│   └── GridRenderer.hpp     # Minimal white theme Raylib grid drawing
├── Engine/
│   └── VisualizerEngine.hpp # Algorithm lifecycle, maze triggers & animation controller
└── UI/
    └── UIManager.hpp        # Clean white sidebar with buttons, controls & status

src/                         # Corresponding .cpp implementations
tests/
├── doctest.h                # doctest v2.4.11 (header-only test framework)
└── test_algorithms.cpp      # 43 headless algorithm & maze tests (657 assertions)
```

### Key Design Decisions

- **GridModel and MazeGenerator are Raylib-independent**: Decoupled core logic can be executed and tested headlessly without an active window or GPU.
- **IPathfinder interface**: Uniform contract (`initialize()`, `step()`, `isComplete()`, `isPathFound()`, `getPath()`) enables polymorphism and hot-swappable algorithms.
- **Incremental stepping**: Each frame performs a configured number of algorithm steps, keeping animation smooth and UI responsive.
- **PathfindingCore static library**: Core logic is compiled once into `libPathfindingCore.a` and linked into both `PathfindingVisualizer` and `PathfindingTests`.

## Test Coverage

The test suite covers:
- ✅ Direct path finding on empty grids
- ✅ Path finding around complex obstacle arrangements
- ✅ Unreachable targets (completely enclosed by walls)
- ✅ Start equals Target edge case
- ✅ Start position on a wall or outside grid bounds
- ✅ Single-cell grids and boundary traversals
- ✅ Repeated runs after state resets
- ✅ Shortest-path optimality verification for BFS, Dijkstra, and A\*
- ✅ Maze generation validity (wall and passage generation, non-empty bounds)
- ✅ Maze solvability across all 4 algorithms (BFS, DFS, Dijkstra, A\*)
- ✅ Deterministic reproduction of mazes with seeded RNG
