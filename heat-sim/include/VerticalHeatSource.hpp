#pragma once

#include "BoundaryCondition.hpp"

class VerticalHeatSource : public BoundaryCondition {
public:
	VerticalHeatSource(int col, double temperature);

	void apply(Grid2D& grid) const override;

private:
	int col_;
	double temperature_;

};