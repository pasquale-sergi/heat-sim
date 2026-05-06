#pragma once

#include "Grid2D.hpp"

class BoundaryCondition {
public:
	virtual ~BoundaryCondition() = default;
	virtual void apply(Grid2D& grid) const = 0;
};