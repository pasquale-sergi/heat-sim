#pragma once

#include "BoundaryCondition.hpp"

class FixedTopBoundary : public BoundaryCondition {
public:
	explicit FixedTopBoundary(double fixedValue);

	void apply(Grid2D& grid) const override;

private:
	double fixedValue_;
};