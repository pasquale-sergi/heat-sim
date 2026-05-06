#include "ContactSheetWriter.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

ContactSheetWriter::ContactSheetWriter(double minValue, double maxValue, int columns)
    : minValue_(minValue),
    maxValue_(maxValue),
    columns_(columns)
{
    if (maxValue <= minValue) {
        throw std::invalid_argument("maxValue must be greater than minValue.");
    }

    if (columns <= 0) {
        throw std::invalid_argument("Number of columns must be positive.");
    }
}

void ContactSheetWriter::write(const Grid2D& grid, int step) const {
    (void)step; // We do not need the step number for now.

    frames_.push_back(gridToImage(grid));
}

void ContactSheetWriter::save(const std::string& filename) const {
    if (frames_.empty()) {
        throw std::runtime_error("Cannot create contact sheet: no frames were collected.");
    }

    const int smallWidth = frames_[0].width();
    const int smallHeight = frames_[0].height();

    for (const auto& frame : frames_) {
        if (frame.width() != smallWidth || frame.height() != smallHeight) {
            throw std::runtime_error("All frames must have the same size.");
        }
    }

    const int rows = static_cast<int>((frames_.size() + columns_ - 1) / columns_);

    PPMImage contactSheet(
        smallWidth * columns_,
        smallHeight * rows
    );

    for (int frameIndex = 0; frameIndex < static_cast<int>(frames_.size()); ++frameIndex) {
        const int sheetCol = frameIndex % columns_;
        const int sheetRow = frameIndex / columns_;

        const int offsetX = sheetCol * smallWidth;
        const int offsetY = sheetRow * smallHeight;

        const PPMImage& frame = frames_[frameIndex];

        for (int y = 0; y < smallHeight; ++y) {
            for (int x = 0; x < smallWidth; ++x) {
                Pixel pixel = frame.getPixel(x, y);

                contactSheet.setPixel(
                    offsetX + x,
                    offsetY + y,
                    pixel
                );
            }
        }
    }

    std::ofstream file(filename);

    if (!file) {
        throw std::runtime_error("Could not open contact sheet output file.");
    }

    file << "P3\n";
    file << contactSheet.width() << " " << contactSheet.height() << "\n";
    file << "255\n";

    for (int y = 0; y < contactSheet.height(); ++y) {
        for (int x = 0; x < contactSheet.width(); ++x) {
            Pixel pixel = contactSheet.getPixel(x, y);

            file << pixel.r << " "
                << pixel.g << " "
                << pixel.b << " ";
        }

        file << "\n";
    }
}

PPMImage ContactSheetWriter::gridToImage(const Grid2D& grid) const {
    PPMImage image(grid.width(), grid.height());

    for (int row = 0; row < grid.height(); ++row) {
        for (int col = 0; col < grid.width(); ++col) {
            const double value = grid.get(row, col);

            const double normalized =
                (value - minValue_) / (maxValue_ - minValue_);

            const double t = std::clamp(normalized, 0.0, 1.0);

            const int red = clampToByte(255.0 * t);
            const int green = 0;
            const int blue = clampToByte(255.0 * (1.0 - t));

            image.setPixel(col, row, Pixel{ red, green, blue });
        }
    }

    return image;
}

int ContactSheetWriter::clampToByte(double value) {
    value = std::clamp(value, 0.0, 255.0);
    return static_cast<int>(value);
}