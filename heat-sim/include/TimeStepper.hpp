//Time stepper header is where we declare an abrastact class that will take care of the different algorithms we might use for simulation
//Independecy is the key here since this class will not relate to one single algorithm
#pragma once

#include "Grid2D.hpp"

class TimeStepper {
public:
	virtual ~TimeStepper() = default;

	virtual void step(
		const Grid2D& current,
		Grid2D& next,
		double alpha,
		double dt,
		double dx
	) const = 0;
};
