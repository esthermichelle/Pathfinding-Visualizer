#pragma once

#include "IPathfinder.hpp"
#include <queue>
#include <vector>
#include <functional>

using namespace std;

struct AStarNode
{
    Position pos;
    double cost;

    bool operator>(const AStarNode &other) const
    {
        return cost > other.cost;
    }
};

class AStar : public IPathfinder
{
public:
    void initialize(GridModel &model, Position start, Position target) override;
    bool step(GridModel &model) override;

    bool isComplete() const override;
    bool isPathFound() const override;
    vector<Position> getPath() const override;

private:
    priority_queue<
        AStarNode,
        vector<AStarNode>,
        greater<AStarNode>
    > frontier;

    Position startPos;
    Position targetPos;

    bool complete = false;
    bool pathFound = false;

    vector<Position> path;

    double heuristic(Position a, Position b) const;

    void reconstructPath(GridModel &model);
};
