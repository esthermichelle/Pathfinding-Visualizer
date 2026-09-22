#pragma once

#include "../Core/Types.hpp"
#include "../Grid/GridModel.hpp"
#include <vector>

class IPathfinder
{
public:
    virtual ~IPathfinder() = default;

    virtual void initialize(GridModel &model, Position start, Position target) = 0;

    virtual bool step(GridModel &model) = 0;

    virtual bool isComplete() const = 0;

    virtual bool isPathFound() const = 0;

    virtual std::vector<Position> getPath() const = 0;
};