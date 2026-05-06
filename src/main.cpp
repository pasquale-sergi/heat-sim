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
#include "Material.hpp"
#include "MaterialLibrary.hpp"

int main(int argc, char* argv[]) {

    try {
        
        std::string materialName = "Aluminum";

        if (argc >= 2) {
            materialName = argv[1];
			std::cout << "Using material from command line: " << materialName << "\n";
		}
        std::cout << "Using material default: " << materialName << "\n";
        
        Material material = MaterialLibrary::findByName(materialName);

        const int width = 200;
        const int height = 200;

        const double alpha = material.thermalDiffusivity();
        const double dt = 0.1;
        const double dx = 0.01;
		const double r = alpha * dt / (dx * dx);

        std::cout << "Material: " << material.name() << "\n";
        std::cout << "Thermal diffusivity: " << alpha << " m^2/s\n";
        std::cout << "r: " << r << "\n";

        Grid2D initialGrid(width, height, 20.0);

        auto stepper = std::make_unique<ExplicitEulerStepper>();

        HeatSimulation simulation(
            std::move(initialGrid),
            std::move(stepper),
            alpha,
            dt,
            dx
        );

        simulation.addBoundaryCondition(
            std::make_unique<FixedTopBoundary>(100.0)
        );

        simulation.addBoundaryCondition(
            std::make_unique<VerticalHeatSource>(width / 2, 100.0)
        );


        ContactSheetWriter writer(20.0, 100.0, 4);

        writer.setTitle("2D Heat Diffusion - " + material.name());
        writer.setMaterialInfo(material, dt, dx);

        simulation.run(1000, writer, 100);

        writer.save("contact_sheet.ppm");

        std::cout << "Contact sheet written to contact_sheet.ppm\n";

        return 0;
    }catch(const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
		
        std::cerr << "Available materials:\n";
		for (const auto& mat : MaterialLibrary::all()) {
            std::cerr << " - " << mat.name() << "\n";
        }
        
        return 1;
    }
}
