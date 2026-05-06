#include "HeatSimulation.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

HeatSimulation::HeatSimulation(
    Grid2D initialGrid,
    std::unique_ptr<TimeStepper> stepper,
    double alpha,
    double dt,
    double dx
)
    : current_(std::move(initialGrid)),
    next_(current_.width(), current_.height(), 0.0),
    stepper_(std::move(stepper)),
    alpha_(alpha),
    dt_(dt),
    dx_(dx)
{
    if (!stepper_) {
        throw std::invalid_argument("TimeStepper cannot be null.");
    }

    if (alpha <= 0.0) {
        throw std::invalid_argument("Alpha must be positive.");
    }

    if (dt <= 0.0) {
        throw std::invalid_argument("dt must be positive.");
    }

    if (dx <= 0.0) {
        throw std::invalid_argument("dx must be positive.");
    }
}

void HeatSimulation::addBoundaryCondition(std::unique_ptr<BoundaryCondition> boundaryCondition) {
    if (!boundaryCondition) {
        throw std::invalid_argument("BoundaryCondition cannot be null.");
    }
    boundaryConditions_.push_back(std::move(boundaryCondition));
}

void HeatSimulation::run(int steps) {
    if (steps < 0) {
        throw std::invalid_argument("Number of steps cannot be negative.");
    }

	applyBoundaryConditions();

    for (int step = 0; step < steps; ++step) {
        stepper_->step(current_, next_, alpha_, dt_, dx_);
		std::swap(current_, next_);
		applyBoundaryConditions();
    }
}

void HeatSimulation::print() const {
    for (int row = 0; row < current_.height(); ++row) {
        for (int col = 0; col < current_.width(); ++col) {
            std::cout << current_.get(row, col) << " ";
        }
        std::cout << "\n";
    }
}

const Grid2D& HeatSimulation::currentGrid() const {
    return current_;
}

void HeatSimulation::applyBoundaryConditions() {
    for (const auto& bc : boundaryConditions_) {
        bc->apply(current_);
    }
}