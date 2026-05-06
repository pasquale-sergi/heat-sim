#include "PpmImageWriter.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

PpmImageWriter::PpmImageWriter(std::string outputDirectory, double minValue, double maxValue)
    : outputDirectory_(std::move(outputDirectory)),
    minValue_(minValue),
    maxValue_(maxValue)
{
    if (maxValue <= minValue) {
        throw std::invalid_argument("maxValue must be greater than minValue.");
    }
}

void PpmImageWriter::write(const Grid2D& grid, int step) const {
    std::ostringstream filename;
    filename << outputDirectory_ << "/heat_step_" << step << ".ppm";

    std::ofstream file(filename.str());

    if (!file) {
        throw std::runtime_error("Could not open output image file.");
    }

    file << "P3\n";
    file << grid.width() << " " << grid.height() << "\n";
    file << "255\n";

    for (int row = 0; row < grid.height(); ++row) {
        for (int col = 0; col < grid.width(); ++col) {
            const double value = grid.get(row, col);

            const double normalized =
                (value - minValue_) / (maxValue_ - minValue_);

            const double t = std::clamp(normalized, 0.0, 1.0);

            // Simple color gradient:
            // cold = blue, hot = red
            const int red = clampToByte(255.0 * t);
            const int green = 0;
            const int blue = clampToByte(255.0 * (1.0 - t));

            file << red << " " << green << " " << blue << " ";
        }

        file << "\n";
    }
}

int PpmImageWriter::clampToByte(double value) {
    value = std::clamp(value, 0.0, 255.0);
    return static_cast<int>(value);
}