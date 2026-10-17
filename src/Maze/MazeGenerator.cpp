#include "MazeGenerator.hpp"
#include <stack>
#include <algorithm>

void MazeGenerator::generate(GridModel &grid, Position start, Position target,
                              unsigned int seed)
{
    int rows = grid.getRows();
    int cols = grid.getCols();

    // Seed the RNG
    std::mt19937 rng(seed == 0 ? std::random_device{}() : seed);

    // Step 1: Fill the entire grid with walls
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            grid.setNodeState({r, c}, NodeState::WALL);

    // The maze uses odd-numbered cells as passages.
    // We carve passages on a sub-grid of odd coordinates.
    // Walls remain on even coordinates.

    // Ensure start and target are at valid (carveable) positions.
    // Snap them to nearest odd cell if needed.
    auto snapOdd = [&](Position pos) -> Position {
        int r = pos.row | 1; // make odd
        int c = pos.col | 1;
        if (r >= rows) r = rows - 2;
        if (c >= cols) c = cols - 2;
        if (r < 1) r = 1;
        if (c < 1) c = 1;
        return {r, c};
    };

    Position mazeStart = snapOdd(start);

    // Step 2: Iterative DFS (recursive backtracker) to carve passages
    // Each "cell" in the maze is at odd coordinates.
    // Moving between cells means jumping 2 cells and carving the wall in between.

    // Track which maze cells have been visited
    // Maze cell (r,c) maps to visited[(r/2)][(c/2)]
    int mazeRows = (rows - 1) / 2;
    int mazeCols = (cols - 1) / 2;

    if (mazeRows < 1 || mazeCols < 1)
    {
        // Grid too small for proper maze, just clear it
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                grid.setNodeState({r, c}, NodeState::EMPTY);
        grid.setNodeState(start, NodeState::START);
        grid.setNodeState(target, NodeState::TARGET);
        return;
    }

    std::vector<std::vector<bool>> visited(mazeRows, std::vector<bool>(mazeCols, false));

    auto toMaze = [](Position p) -> Position {
        return {p.row / 2, p.col / 2};
    };

    auto mazeInBounds = [&](int mr, int mc) -> bool {
        return mr >= 0 && mr < mazeRows && mc >= 0 && mc < mazeCols;
    };

    // Directions: up, down, left, right (in maze coordinates, step=1)
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};

    std::stack<Position> stack;
    Position mStart = toMaze(mazeStart);

    // Carve the starting cell
    grid.setNodeState(mazeStart, NodeState::EMPTY);
    visited[mStart.row][mStart.col] = true;
    stack.push(mStart);

    while (!stack.empty())
    {
        Position current = stack.top();

        // Find unvisited neighbors
        std::vector<int> neighbors;
        for (int d = 0; d < 4; d++)
        {
            int nr = current.row + dr[d];
            int nc = current.col + dc[d];
            if (mazeInBounds(nr, nc) && !visited[nr][nc])
                neighbors.push_back(d);
        }

        if (neighbors.empty())
        {
            stack.pop();
            continue;
        }

        // Pick a random unvisited neighbor
        std::uniform_int_distribution<int> dist(0, static_cast<int>(neighbors.size()) - 1);
        int chosen = neighbors[dist(rng)];

        int nr = current.row + dr[chosen];
        int nc = current.col + dc[chosen];

        // Carve the wall between current and neighbor
        int wallR = current.row * 2 + 1 + dr[chosen];
        int wallC = current.col * 2 + 1 + dc[chosen];
        int cellR = nr * 2 + 1;
        int cellC = nc * 2 + 1;

        if (grid.inBounds({wallR, wallC}))
            grid.setNodeState({wallR, wallC}, NodeState::EMPTY);
        if (grid.inBounds({cellR, cellC}))
            grid.setNodeState({cellR, cellC}, NodeState::EMPTY);

        visited[nr][nc] = true;
        stack.push({nr, nc});
    }

    // Step 3: Connect start and target to the maze network
    // The maze spans all odd-coordinate cells. By connecting any start/target
    // to its nearest odd-coordinate cell, we guarantee a continuous path exists.
    auto connectToMaze = [&](Position pos) {
        if (!grid.inBounds(pos))
            return;

        int targetR = pos.row;
        if (targetR % 2 == 0)
            targetR = (targetR + 1 < rows) ? targetR + 1 : targetR - 1;

        int targetC = pos.col;
        if (targetC % 2 == 0)
            targetC = (targetC + 1 < cols) ? targetC + 1 : targetC - 1;

        int curR = pos.row;
        int curC = pos.col;

        while (curR != targetR)
        {
            grid.setNodeState({curR, curC}, NodeState::EMPTY);
            curR += (targetR > curR) ? 1 : -1;
        }
        while (curC != targetC)
        {
            grid.setNodeState({curR, curC}, NodeState::EMPTY);
            curC += (targetC > curC) ? 1 : -1;
        }
        grid.setNodeState({targetR, targetC}, NodeState::EMPTY);
    };

    connectToMaze(start);
    connectToMaze(target);

    // Step 4: Set start and target states
    grid.setNodeState(start, NodeState::START);
    grid.setNodeState(target, NodeState::TARGET);
}
