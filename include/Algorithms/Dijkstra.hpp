#pragma once

#include "IPathfinder.hpp"
#include <queue>
#include <vector>
#include <functional>

using namespace std;

struct DijkstraNode
{
    Position pos;
    double cost;

    bool operator>(const DijkstraNode &other) const
    {
        return cost > other.cost;
    }
};

class Dijkstra : public IPathfinder
{
public:
    void initialize(GridModel &model, Position start, Position target) override;
    bool step(GridModel &model) override;

    bool isComplete() const override;
    bool isPathFound() const override;
    vector<Position> getPath() const override;

private:
    priority_queue<
        DijkstraNode,
        vector<DijkstraNode>,
        greater<DijkstraNode>
    > frontier;

    Position startPos;
    Position targetPos;

    bool complete = false;
    bool pathFound = false;

    vector<Position> path;

    void reconstructPath(GridModel &model);
};
