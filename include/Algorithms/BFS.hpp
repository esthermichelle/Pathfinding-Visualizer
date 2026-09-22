#pragma once

#include "IPathfinder.hpp"
#include <queue>
using namespace std;

class BFS : public IPathfinder
{
public:
    void initialize(GridModel &model, Position start, Position target) override;
    bool step(GridModel &model) override;

    bool isComplete() const override;
    bool isPathFound() const override;
    vector<Position> getPath() const override;

private:
    queue<Position> frontier; 

    Position startPos;
    Position targetPos;

    bool complete;
    bool pathFound;
};