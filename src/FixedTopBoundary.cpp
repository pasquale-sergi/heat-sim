#include "FixedTopBoundary.hpp"

FixedTopBoundary::FixedTopBoundary(double fixedValue) : fixedValue_(fixedValue) {}

void FixedTopBoundary::apply(Grid2D& grid) const {
	for (int col = 0; col < grid.width(); ++col) {
		grid.set(0, col, fixedValue_);
	}
}

