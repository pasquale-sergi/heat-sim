#pragma once

#include <memory>
#include "Grid2D.hpp"
#include "TimeStepper.hpp"
#include <vector>
#include <memory>
#include "BoundaryCondition.hpp"

class HeatSimulation {
public:
    HeatSimulation(
        Grid2D initialGrid,
        std::unique_ptr<TimeStepper> stepper,
        double alpha,
        double dt,
        double dx
    );

	void addBoundaryCondition(std::unique_ptr<BoundaryCondition> boundaryCondition);

    void run(int steps);
    void print() const;

    const Grid2D& currentGrid() const;

private:
	void applyBoundaryConditions();

    Grid2D current_;
    Grid2D next_;
    std::unique_ptr<TimeStepper> stepper_;
	std::vector<std::unique_ptr<BoundaryCondition>> boundaryConditions_;

    double alpha_;
    double dt_;
    double dx_;
};