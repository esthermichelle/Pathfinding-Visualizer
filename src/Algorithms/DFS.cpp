#include "DFS.hpp"
#include <algorithm>

void DFS::initialize(
    GridModel &model,
    Position start,
    Position target
)
{
    startPos = start;
    targetPos = target;

    frontier = stack<Position>();

    complete = false;
    pathFound = false;

    path.clear();

    if (!model.inBounds(start) ||
        !model.inBounds(target) ||
        model.isWall(start))
    {
        complete = true;
        pathFound = false;
        return;
    }

    frontier.push(startPos);

    if (startPos == targetPos)
    {
        complete = true;
        pathFound = true;
        path.push_back(startPos);
        return;
    }
}

bool DFS::step(GridModel &model)
{
    if (complete)
        return true;

    if (frontier.empty())
    {
        complete = true;
        pathFound = false;
        return true;
    }

    Position current = frontier.top();
    frontier.pop();

    Node &currentNode = model.getNode(current);

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

        if (neighbor.state == NodeState::WALL ||
            neighbor.state == NodeState::VISITED ||
            neighbor.state == NodeState::FRONTIER ||
            neighbor.state == NodeState::START)
        {
            continue;
        }

        neighbor.parent = current;

        if (neighbor.state == NodeState::TARGET)
        {
            complete = true;
            pathFound = true;

            reconstructPath(model);

            return true;
        }

        neighbor.state = NodeState::FRONTIER;

        frontier.push(next);
    }

    return false;
}

void DFS::reconstructPath(GridModel &model)
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

bool DFS::isComplete() const
{
    return complete;
}

bool DFS::isPathFound() const
{
    return pathFound;
}

vector<Position> DFS::getPath() const
{
    return path;
}
