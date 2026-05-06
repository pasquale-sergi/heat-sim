#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "BoundaryCondition.hpp"
#include "ComparisonSheetWriter.hpp"
#include "ContactSheetWriter.hpp"
#include "ExplicitEulerStepper.hpp"
#include "FixedTopBoundary.hpp"
#include "FrameCollector.hpp"
#include "Grid2D.hpp"
#include "HeatSimulation.hpp"
#include "Material.hpp"
#include "MaterialLibrary.hpp"
#include "VerticalHeatSource.hpp"

namespace {
    void printMaterialInfo(const Material& material, double dt, double dx) {
        const double alpha = material.thermalDiffusivity();
        const double r = alpha * dt / (dx * dx);

        std::cout << "Material: " << material.name() << "\n";
        std::cout << "Thermal diffusivity: " << alpha << " m^2/s\n";
        std::cout << "r: " << r << "\n";
    }

    HeatSimulation createSimulation(
        const Material& material,
        int width,
        int height,
        double initialTemperature,
        double hotTemperature,
        double dt,
        double dx
    ) {
        const double alpha = material.thermalDiffusivity();

        Grid2D initialGrid(width, height, initialTemperature);

        auto stepper = std::make_unique<ExplicitEulerStepper>();

        HeatSimulation simulation(
            std::move(initialGrid),
            std::move(stepper),
            alpha,
            dt,
            dx
        );

        simulation.addBoundaryCondition(
            std::make_unique<FixedTopBoundary>(hotTemperature)
        );

        simulation.addBoundaryCondition(
            std::make_unique<VerticalHeatSource>(width / 2, hotTemperature)
        );

        return simulation;
    }

    void runSingleMaterialSimulation(
        const Material& material,
        int width,
        int height,
        int totalSteps,
        int outputEvery,
        double initialTemperature,
        double hotTemperature,
        double dt,
        double dx
    ) {
        printMaterialInfo(material, dt, dx);

        HeatSimulation simulation = createSimulation(
            material,
            width,
            height,
            initialTemperature,
            hotTemperature,
            dt,
            dx
        );

        ContactSheetWriter writer(initialTemperature, hotTemperature, 4);

        writer.setTitle("2D Heat Diffusion - " + material.name());
        writer.setMaterialInfo(material, dt, dx);

        simulation.run(totalSteps, writer, outputEvery);

        const std::string outputFile =
            "contact_sheet_" + material.name() + ".ppm";

        writer.save(outputFile);

        std::cout << "Current path: "
            << std::filesystem::current_path()
            << "\n";

        std::cout << "Contact sheet written to "
            << outputFile
            << "\n";
    }

    FrameCollector runComparisonSimulation(
        const Material& material,
        int width,
        int height,
        int totalSteps,
        int outputEvery,
        double initialTemperature,
        double hotTemperature,
        double dt,
        double dx
    ) {
        printMaterialInfo(material, dt, dx);

        HeatSimulation simulation = createSimulation(
            material,
            width,
            height,
            initialTemperature,
            hotTemperature,
            dt,
            dx
        );

        FrameCollector collector(initialTemperature, hotTemperature);

        simulation.run(totalSteps, collector, outputEvery);

        return collector;
    }

    void printUsage() {
        std::cerr << "Available materials:\n";

        for (const auto& material : MaterialLibrary::all()) {
            std::cerr << "- " << material.name() << "\n";
        }

        std::cerr << "\nExamples:\n";
        std::cerr << "heat_sim\n";
        std::cerr << "heat_sim Aluminum\n";
        std::cerr << "heat_sim Copper\n";
        std::cerr << "heat_sim compare Copper Ceramic\n";
    }
}

int main(int argc, char* argv[]) {
    try {
        const int width = 200;
        const int height = 200;

        const int totalSteps = 1000;
        const int outputEvery = 100;

        const double initialTemperature = 20.0;
        const double hotTemperature = 100.0;

        const double dt = 0.1;
        const double dx = 0.01;

        if (argc >= 2 && std::string(argv[1]) == "compare") {
            if (argc < 4) {
                throw std::invalid_argument(
                    "Usage: heat_sim compare MaterialA MaterialB"
                );
            }

            Material firstMaterial = MaterialLibrary::findByName(argv[2]);
            Material secondMaterial = MaterialLibrary::findByName(argv[3]);

            FrameCollector firstFrames = runComparisonSimulation(
                firstMaterial,
                width,
                height,
                totalSteps,
                outputEvery,
                initialTemperature,
                hotTemperature,
                dt,
                dx
            );

            FrameCollector secondFrames = runComparisonSimulation(
                secondMaterial,
                width,
                height,
                totalSteps,
                outputEvery,
                initialTemperature,
                hotTemperature,
                dt,
                dx
            );

            ComparisonSheetWriter comparisonWriter;

            const std::string outputFile =
                "comparison_" +
                firstMaterial.name() +
                "_vs_" +
                secondMaterial.name() +
                ".ppm";

            comparisonWriter.save(
                firstMaterial,
                firstFrames,
                secondMaterial,
                secondFrames,
                dt,
                outputFile
            );

            std::cout << "Current path: "
                << std::filesystem::current_path()
                << "\n";

            std::cout << "Comparison written to "
                << outputFile
                << "\n";

            return 0;
        }

        std::string materialName = "Aluminum";

        if (argc >= 2) {
            materialName = argv[1];
        }

        Material material = MaterialLibrary::findByName(materialName);

        runSingleMaterialSimulation(
            material,
            width,
            height,
            totalSteps,
            outputEvery,
            initialTemperature,
            hotTemperature,
            dt,
            dx
        );

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << "\n\n";
        printUsage();
        return 1;
    }
}