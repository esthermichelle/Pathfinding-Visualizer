#include "BFS.hpp"

void BFS::initialize(GridModel &model, Position start, Position target)
{
    startPos = start;
    targetPos = target;

    frontier = std::queue<Position>();
    complete = false;
    pathFound = false;

    frontier.push(startPos);
}

bool BFS::step(GridModel &model)
{
    if (complete)
        return true;

    if (frontier.empty())
    {
        complete = true;
        pathFound = false;
        return true;
    }

    Position current = frontier.front();
    frontier.pop();

    Node &currentNode = model.getNode(current);

    if (current != startPos)
        currentNode.state = NodeState::VISITED;

    if (current == targetPos)
    {
        complete = true;
        pathFound = true;
        return true;
    }

    std::vector<Position> directions = {
        {current.row - 1, current.col}, // up
        {current.row + 1, current.col}, // down
        {current.row, current.col - 1}, // left
        {current.row, current.col + 1}  // right
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
            continue;

        neighbor.parent = current;

        if (neighbor.state == NodeState::TARGET)
        {
            complete = true;
            pathFound = true;
            return true;
        }

        frontier.push(next);

        neighbor.state = NodeState::FRONTIER;
    }

    return false;
}

bool BFS::isComplete() const
{
    return complete;
}

bool BFS::isPathFound() const
{
    return pathFound;
}