#include <filesystem>
#include <iostream>
#include <memory>

#include "Grid2D.hpp"
#include "ExplicitEulerStepper.hpp"
#include "FixedTopBoundary.hpp"
#include "HeatSimulation.hpp"
#include "PpmImageWriter.hpp"

int main() {
    Grid2D initialGrid(200, 200, 20.0);

    auto stepper = std::make_unique<ExplicitEulerStepper>();

    HeatSimulation simulation(
        std::move(initialGrid),
        std::move(stepper),
        0.5,
        0.1,
        1.0
    );

    simulation.addBoundaryCondition(
        std::make_unique<FixedTopBoundary>(100.0)
    );

    simulation.run(2000);

    PpmImageWriter writer(".", 20.0, 100.0);
    writer.write(simulation.currentGrid(), 2000);

    std::cout << "Current path: "
        << std::filesystem::current_path()
        << "\n";

    std::cout << "Image written: heat_step_2000.ppm\n";

    return 0;
}