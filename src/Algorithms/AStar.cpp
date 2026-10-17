#include "AStar.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

double AStar::heuristic(Position a, Position b) const
{
    return abs(a.row - b.row) +
           abs(a.col - b.col);
}

void AStar::initialize(
    GridModel &model,
    Position start,
    Position target
)
{
    startPos = start;
    targetPos = target;

    frontier = priority_queue<
        AStarNode,
        vector<AStarNode>,
        greater<AStarNode>
    >();

    complete = false;
    pathFound = false;

    path.clear();

    model.resetStates();

    if (!model.inBounds(start) ||
        !model.inBounds(target) ||
        model.isWall(start))
    {
        complete = true;
        return;
    }

    Node &startNode = model.getNode(start);

    startNode.gCost = 0.0;
    startNode.hCost = heuristic(start, target);
    startNode.parent = {-1, -1};

    frontier.push({
        start,
        startNode.fCost()
    });

    if (start == target)
    {
        complete = true;
        pathFound = true;
        path.push_back(start);
        return;
    }
}

bool AStar::step(GridModel &model)
{
    if (complete)
        return true;

    if (frontier.empty())
    {
        complete = true;
        pathFound = false;
        return true;
    }

    AStarNode currentEntry = frontier.top();
    frontier.pop();

    Position current = currentEntry.pos;

    Node &currentNode = model.getNode(current);

    if (currentEntry.cost > currentNode.fCost())
        return false;

    if (current != startPos)
        currentNode.state = NodeState::VISITED;

    if (current == targetPos)
    {
        complete = true;
        pathFound = true;
        reconstructPath(model);
        return true;
    }

    vector<Position> directions = {
        {current.row - 1, current.col},
        {current.row + 1, current.col},
        {current.row, current.col - 1},
        {current.row, current.col + 1}
    };

    for (auto &next : directions)
    {
        if (!model.inBounds(next))
            continue;

        Node &neighbor = model.getNode(next);

        if (neighbor.state == NodeState::WALL)
            continue;

        double newGCost = currentNode.gCost + 1.0;

        if (newGCost < neighbor.gCost)
        {
            neighbor.gCost = newGCost;
            neighbor.hCost = heuristic(next, targetPos);
            neighbor.parent = current;

            if (neighbor.state != NodeState::TARGET)
                neighbor.state = NodeState::FRONTIER;

            frontier.push({
                next,
                neighbor.fCost()
            });
        }
    }

    return false;
}

void AStar::reconstructPath(GridModel &model)
{
    path.clear();

    Position curr = targetPos;

    while (
        curr != startPos &&
        curr.row != -1 &&
        curr.col != -1
    )
    {
        path.push_back(curr);
        curr = model.getNode(curr).parent;
    }

    if (curr == startPos)
        path.push_back(startPos);

    reverse(path.begin(), path.end());
}

bool AStar::isComplete() const
{
    return complete;
}

bool AStar::isPathFound() const
{
    return pathFound;
}

vector<Position> AStar::getPath() const
{
    return path;
}
