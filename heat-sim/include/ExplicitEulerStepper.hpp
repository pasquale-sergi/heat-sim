#pragma once

#include "Grid2D.hpp"
#include "TimeStepper.hpp"

class ExplicitEulerStepper : public TimeStepper {
public:
	void step(
		const Grid2D& current,
		Grid2D& next,
		double alpha,
		double dt,
		double dx
	) const override;
};