# Pathfinding Visualizer

An interactive 2D pathfinding visualization tool built with **C++17** and **Raylib**. Watch BFS, DFS, Dijkstra and A\* explore a grid step-by-step, place walls, drag endpoints, and compare algorithms in real time.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Raylib](https://img.shields.io/badge/Raylib-5.5-green)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-red)

## Features

- **Four algorithms**: BFS, DFS, Dijkstra, A\* (Manhattan heuristic)
- **Step-by-step animation**: Non-blocking incremental visualization at configurable speed
- **Interactive grid**: Left-click to place walls, right-click to erase
- **Draggable endpoints**: Click and drag Start (S) or Target (T) to reposition
- **Full controls**: Start, Pause, Resume, Clear Path, Reset Grid
- **Speed control**: Adjust steps-per-frame from 1 to 50
- **Dark theme UI**: Modern sidebar with algorithm selection, controls and status
- **Headless test suite**: 39 tests covering correctness, edge cases and shortest-path guarantees

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
| Select algorithm | Click algorithm button in sidebar |
| Start visualization | Click **Start** button |
| Pause/Resume | Click **Pause** / **Resume** button |
| Clear path | Click **Clear Path** (keeps walls) |
| Reset grid | Click **Reset Grid** (clears everything) |
| Adjust speed | Click **Slower** / **Faster** buttons |
| Quit | Close window or press Escape |

## Algorithms

| Algorithm | Optimal? | Data Structure | Description |
|-----------|----------|---------------|-------------|
| **BFS** | ✅ Yes | Queue | Explores level-by-level; guarantees shortest path on unweighted grids |
| **DFS** | ❌ No | Stack | Explores depth-first; finds *a* path but not necessarily the shortest |
| **Dijkstra** | ✅ Yes | Priority Queue | Finds shortest path using cumulative cost; handles stale PQ entries |
| **A\*** | ✅ Yes | Priority Queue | Dijkstra + Manhattan heuristic; optimal and typically faster than Dijkstra |

All algorithms use **4-directional movement** (up, down, left, right) with **equal edge costs** (1.0).

## Architecture

```
include/
├── Core/
│   └── Types.hpp           # NodeState enum, Position struct
├── Grid/
│   ├── Node.hpp            # Node with pos, state, parent, costs
│   └── GridModel.hpp       # Graphics-independent grid data model
├── Algorithms/
│   ├── IPathfinder.hpp     # Abstract interface for all algorithms
│   ├── BFS.hpp
│   ├── DFS.hpp
│   ├── Dijkstra.hpp
│   └── AStar.hpp
├── Rendering/
│   └── GridRenderer.hpp    # Raylib grid drawing
├── Engine/
│   └── VisualizerEngine.hpp # Algorithm lifecycle & animation controller
└── UI/
    └── UIManager.hpp       # Sidebar panel with buttons & status

src/                        # Corresponding .cpp implementations
tests/
├── doctest.h               # doctest v2.4.11 (header-only test framework)
└── test_algorithms.cpp     # 39 headless algorithm tests
```

### Key Design Decisions

- **GridModel is Raylib-independent**: Can be tested headlessly without a graphics context
- **IPathfinder interface**: All algorithms implement `initialize()`, `step()`, `isComplete()`, `isPathFound()`, `getPath()` for uniform usage
- **Incremental stepping**: Each `step()` call processes one node, keeping the UI responsive
- **PathfindingCore library**: Static library containing all non-graphics code, shared by both the visualizer and test executables

## Test Coverage

The test suite covers each algorithm independently with these scenarios:

- ✅ Direct path on empty grid
- ✅ Path around obstacles
- ✅ Unreachable target (surrounded by walls)
- ✅ Start equals target
- ✅ Start position is a wall
- ✅ Out-of-bounds start position
- ✅ Single-cell grid
- ✅ Boundary path (blocked center)
- ✅ Repeated runs after grid reset

BFS, Dijkstra and A\* additionally verify **shortest-path correctness** (path length == Manhattan distance + 1 on empty grids).

## License

See [LICENSE](LICENSE) for details.
