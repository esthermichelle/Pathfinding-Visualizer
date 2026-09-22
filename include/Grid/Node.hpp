#pragma once

#include "../Core/Types.hpp"
#include <limits>
using namespace std;

struct Node
{
    Position pos;
    NodeState state = NodeState::EMPTY;
    Position parent = {-1, -1};

    double gCost = std::numeric_limits<double>::infinity();
    double hCost = std::numeric_limits<double>::infinity();

    double fCost() const { return gCost + hCost; }
};