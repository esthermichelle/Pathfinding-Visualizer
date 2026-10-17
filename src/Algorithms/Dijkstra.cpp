#include "Dijkstra.hpp"
#include <algorithm>
#include <limits>

void Dijkstra::initialize(
    GridModel &model,
    Position start,
    Position target
)
{
    startPos = start;
    targetPos = target;

    frontier = priority_queue<
        DijkstraNode,
        vector<DijkstraNode>,
        greater<DijkstraNode>
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
    startNode.parent = {-1, -1};

    frontier.push({start, 0.0});

    if (start == target)
    {
        complete = true;
        pathFound = true;
        path.push_back(start);
        return;
    }
}

bool Dijkstra::step(GridModel &model)
{
    if (complete)
        return true;

    if (frontier.empty())
    {
        complete = true;
        pathFound = false;
        return true;
    }

    DijkstraNode currentEntry = frontier.top();
    frontier.pop();

    Position current = currentEntry.pos;

    Node &currentNode = model.getNode(current);

    if (currentEntry.cost > currentNode.gCost)
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

        double newCost = currentNode.gCost + 1.0;

        if (newCost < neighbor.gCost)
        {
            neighbor.gCost = newCost;
            neighbor.parent = current;

            if (neighbor.state != NodeState::TARGET)
                neighbor.state = NodeState::FRONTIER;

            frontier.push({next, newCost});
        }
    }

    return false;
}

void Dijkstra::reconstructPath(GridModel &model)
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

bool Dijkstra::isComplete() const
{
    return complete;
}

bool Dijkstra::isPathFound() const
{
    return pathFound;
}

vector<Position> Dijkstra::getPath() const
{
    return path;
}
