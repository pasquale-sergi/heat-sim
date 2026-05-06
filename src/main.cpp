#include <iostream>
#include <memory>

#include "Grid2D.hpp"
#include "ExplicitEulerStepper.hpp"
#include "HeatSimulation.hpp"
#include "PpmImageWriter.hpp"

int main() {
    Grid2D initialGrid(200, 200, 20.0);

    for (int col = 0; col < initialGrid.width(); ++col) {
        initialGrid.set(0, col, 100.0);
    }

    auto stepper = std::make_unique<ExplicitEulerStepper>();

    HeatSimulation simulation(
        std::move(initialGrid),
        std::move(stepper),
        0.1,
        0.1,
        1.0
    );

    simulation.run(2000);

    PpmImageWriter writer(".", 20.0, 100.0);
    writer.write(simulation.currentGrid(), 2000);

    std::cout << "Image written: heat_step_100.ppm\n";

    return 0;
}