#include <filesystem>
#include <iostream>
#include <memory>

#include "Grid2D.hpp"
#include "ExplicitEulerStepper.hpp"
#include "FixedTopBoundary.hpp"
#include "VerticalHeatSource.hpp"
#include "HeatSimulation.hpp"
#include "PpmImageWriter.hpp"

#include "ContactSheetWriter.hpp"

int main() {
    const int width = 200;
    const int height = 200;

    Grid2D initialGrid(width, height, 20.0);

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

    simulation.addBoundaryCondition(
        std::make_unique<VerticalHeatSource>(width / 2, 100.0)
    );


    ContactSheetWriter writer(20.0, 100.0, 4);

    simulation.run(1000, writer, 100);

    writer.save("contact_sheet.ppm");

    std::cout << "Contact sheet written to contact_sheet.ppm\n";

    std::cout << "Frames written every 100 steps.\n";

    return 0;
}