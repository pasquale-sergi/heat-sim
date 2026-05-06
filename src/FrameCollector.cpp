#include "FrameCollector.hpp"

#include <algorithm>
#include <stdexcept>

FrameCollector::FrameCollector(double minValue, double maxValue)
    : minValue_(minValue),
    maxValue_(maxValue)
{
    if (maxValue <= minValue) {
        throw std::invalid_argument("maxValue must be greater than minValue.");
    }
}

void FrameCollector::write(const Grid2D& grid, int step) const {
    frames_.push_back(gridToImage(grid));
    steps_.push_back(step);
}

const std::vector<PPMImage>& FrameCollector::frames() const {
    return frames_;
}

const std::vector<int>& FrameCollector::steps() const {
    return steps_;
}

PPMImage FrameCollector::gridToImage(const Grid2D& grid) const {
    PPMImage image(grid.width(), grid.height());

    for (int row = 0; row < grid.height(); ++row) {
        for (int col = 0; col < grid.width(); ++col) {
            const double value = grid.get(row, col);

            const double normalized =
                (value - minValue_) / (maxValue_ - minValue_);

            const double t = std::clamp(normalized, 0.0, 1.0);

            int red = 0;
            int green = 0;
            int blue = 0;

            // blue -> cyan -> green -> yellow -> red
            if (t < 0.25) {
                const double local = t / 0.25;
                red = 0;
                green = clampToByte(255.0 * local);
                blue = 255;
            }
            else if (t < 0.50) {
                const double local = (t - 0.25) / 0.25;
                red = 0;
                green = 255;
                blue = clampToByte(255.0 * (1.0 - local));
            }
            else if (t < 0.75) {
                const double local = (t - 0.50) / 0.25;
                red = clampToByte(255.0 * local);
                green = 255;
                blue = 0;
            }
            else {
                const double local = (t - 0.75) / 0.25;
                red = 255;
                green = clampToByte(255.0 * (1.0 - local));
                blue = 0;
            }

            image.setPixel(col, row, Pixel{ red, green, blue });
        }
    }

    return image;
}

int FrameCollector::clampToByte(double value) {
    value = std::clamp(value, 0.0, 255.0);
    return static_cast<int>(value);
}