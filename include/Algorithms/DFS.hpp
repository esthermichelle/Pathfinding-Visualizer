#pragma once

#include "IPathfinder.hpp"
#include <stack>
#include <vector>

using namespace std;

class DFS : public IPathfinder
{
public:
    void initialize(GridModel &model, Position start, Position target) override;
    bool step(GridModel &model) override;

    bool isComplete() const override;
    bool isPathFound() const override;
    vector<Position> getPath() const override;

private:
    stack<Position> frontier;

    Position startPos;
    Position targetPos;

    bool complete = false;
    bool pathFound = false;

    vector<Position> path;

    void reconstructPath(GridModel &model);
};
