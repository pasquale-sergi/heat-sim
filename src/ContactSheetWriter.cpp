#include "ContactSheetWriter.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

ContactSheetWriter::ContactSheetWriter(double minValue, double maxValue, int columns)
    : minValue_(minValue),
    maxValue_(maxValue),
    columns_(columns),
    title_("2D Heat Simulation")
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
	steps_.push_back(step);
}

void ContactSheetWriter::setTitle(std::string title) {
	title_ = std::move(title);
}

void ContactSheetWriter::setMaterialInfo(const Material& material, double dt, double dx) {
    std::ostringstream alphaStream;
    alphaStream << std::scientific << std::setprecision(3)
        << material.thermalDiffusivity();

    std::ostringstream line1;
    line1 << "Material: " << material.name();

    std::ostringstream line2;
    line2 << "k=" << material.thermalConductivity()
        << " W/(mK), rho=" << material.density()
        << " kg/m3, cp=" << material.specificHeat()
        << " J/(kgK)";

    std::ostringstream line3;
    line3 << "alpha=" << alphaStream.str()
        << " m2/s, dt=" << dt
        << " s, dx=" << dx
        << " m";

    infoLines_.clear();
    infoLines_.push_back(line1.str());
    infoLines_.push_back(line2.str());
    infoLines_.push_back(line3.str());
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

    const int titleHeight = 100;
    const int captionHeight = 28;
    const int padding = 12;

    const int sheetWidth = smallWidth * columns_;
    const int sheetHeight =
        titleHeight + rows * (smallHeight + captionHeight) + padding;

    PPMImage contactSheet(sheetWidth, sheetHeight);

    // Fill background white
    for (int y = 0; y < contactSheet.height(); ++y) {
        for (int x = 0; x < contactSheet.width(); ++x) {
            contactSheet.setPixel(x, y, Pixel{ 255, 255, 255 });
        }
    }

    const Pixel black{ 0, 0, 0 };

    drawText(contactSheet, 12, 12, title_, black, 2);

    int infoY = 40;
    for (const auto& line : infoLines_) {
        drawText(contactSheet, 12, infoY, line, black, 1);
        infoY += 16;
    }

    const int framesStartY = titleHeight;

    for (int frameIndex = 0; frameIndex < static_cast<int>(frames_.size()); ++frameIndex) {
        const int sheetCol = frameIndex % columns_;
        const int sheetRow = frameIndex / columns_;

        const int offsetX = sheetCol * smallWidth;
        const int offsetY = framesStartY + sheetRow * (smallHeight + captionHeight);

        const PPMImage& frame = frames_[frameIndex];

        for (int y = 0; y < smallHeight; ++y) {
            for (int x = 0; x < smallWidth; ++x) {
                contactSheet.setPixel(
                    offsetX + x,
                    offsetY + y,
                    frame.getPixel(x, y)
                );
            }
        }

        std::ostringstream caption;
        caption << "Step " << steps_[frameIndex];

        drawText(
            contactSheet,
            offsetX + 8,
            offsetY + smallHeight + 6,
            caption.str(),
            black,
            1
        );
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

            // Improved heatmap:
            // blue -> cyan -> green -> yellow -> red
            int red = 0;
            int green = 0;
            int blue = 0;

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

int ContactSheetWriter::clampToByte(double value) {
    value = std::clamp(value, 0.0, 255.0);
    return static_cast<int>(value);
}

void ContactSheetWriter::drawText(
    PPMImage& image,
    int x,
    int y,
    const std::string& text,
    Pixel color,
    int scale
) {
    int cursorX = x;

    for (char c : text) {
        drawChar(image, cursorX, y, c, color, scale);
        cursorX += 6 * scale;
    }
}

void ContactSheetWriter::drawChar(
    PPMImage& image,
    int x,
    int y,
    char c,
    Pixel color,
    int scale
) {
    const auto& glyph = glyphFor(c);

    for (int row = 0; row < static_cast<int>(glyph.size()); ++row) {
        for (int col = 0; col < static_cast<int>(glyph[row].size()); ++col) {
            if (glyph[row][col] != '1') {
                continue;
            }

            for (int dy = 0; dy < scale; ++dy) {
                for (int dx = 0; dx < scale; ++dx) {
                    const int px = x + col * scale + dx;
                    const int py = y + row * scale + dy;

                    if (px >= 0 && px < image.width() &&
                        py >= 0 && py < image.height()) {
                        image.setPixel(px, py, color);
                    }
                }
            }
        }
    }
}

const std::vector<std::string>& ContactSheetWriter::glyphFor(char c) {
    static const std::vector<std::string> unknown = {
        "11111",
        "00001",
        "00010",
        "00100",
        "00100",
        "00000",
        "00100"
    };

    static const std::unordered_map<char, std::vector<std::string>> font = {
        {' ', {"00000","00000","00000","00000","00000","00000","00000"}},
        {':', {"00000","00100","00100","00000","00100","00100","00000"}},
        {'.', {"00000","00000","00000","00000","00000","00100","00100"}},
        {',', {"00000","00000","00000","00000","00100","00100","01000"}},
        {'-', {"00000","00000","00000","11111","00000","00000","00000"}},
        {'=', {"00000","11111","00000","11111","00000","00000","00000"}},
        {'/', {"00001","00010","00100","01000","10000","00000","00000"}},
        {'(', {"00010","00100","01000","01000","01000","00100","00010"}},
        {')', {"01000","00100","00010","00010","00010","00100","01000"}},

        {'0', {"01110","10001","10011","10101","11001","10001","01110"}},
        {'1', {"00100","01100","00100","00100","00100","00100","01110"}},
        {'2', {"01110","10001","00001","00010","00100","01000","11111"}},
        {'3', {"11110","00001","00001","01110","00001","00001","11110"}},
        {'4', {"00010","00110","01010","10010","11111","00010","00010"}},
        {'5', {"11111","10000","10000","11110","00001","00001","11110"}},
        {'6', {"01110","10000","10000","11110","10001","10001","01110"}},
        {'7', {"11111","00001","00010","00100","01000","01000","01000"}},
        {'8', {"01110","10001","10001","01110","10001","10001","01110"}},
        {'9', {"01110","10001","10001","01111","00001","00001","01110"}},

        {'A', {"01110","10001","10001","11111","10001","10001","10001"}},
        {'B', {"11110","10001","10001","11110","10001","10001","11110"}},
        {'C', {"01110","10001","10000","10000","10000","10001","01110"}},
        {'D', {"11110","10001","10001","10001","10001","10001","11110"}},
        {'E', {"11111","10000","10000","11110","10000","10000","11111"}},
        {'F', {"11111","10000","10000","11110","10000","10000","10000"}},
        {'G', {"01110","10001","10000","10111","10001","10001","01110"}},
        {'H', {"10001","10001","10001","11111","10001","10001","10001"}},
        {'I', {"01110","00100","00100","00100","00100","00100","01110"}},
        {'J', {"00111","00010","00010","00010","10010","10010","01100"}},
        {'K', {"10001","10010","10100","11000","10100","10010","10001"}},
        {'L', {"10000","10000","10000","10000","10000","10000","11111"}},
        {'M', {"10001","11011","10101","10101","10001","10001","10001"}},
        {'N', {"10001","11001","10101","10011","10001","10001","10001"}},
        {'O', {"01110","10001","10001","10001","10001","10001","01110"}},
        {'P', {"11110","10001","10001","11110","10000","10000","10000"}},
        {'Q', {"01110","10001","10001","10001","10101","10010","01101"}},
        {'R', {"11110","10001","10001","11110","10100","10010","10001"}},
        {'S', {"01111","10000","10000","01110","00001","00001","11110"}},
        {'T', {"11111","00100","00100","00100","00100","00100","00100"}},
        {'U', {"10001","10001","10001","10001","10001","10001","01110"}},
        {'V', {"10001","10001","10001","10001","10001","01010","00100"}},
        {'W', {"10001","10001","10001","10101","10101","10101","01010"}},
        {'X', {"10001","10001","01010","00100","01010","10001","10001"}},
        {'Y', {"10001","10001","01010","00100","00100","00100","00100"}},
        {'Z', {"11111","00001","00010","00100","01000","10000","11111"}},

        {'a', {"00000","00000","01110","00001","01111","10001","01111"}},
        {'b', {"10000","10000","10110","11001","10001","10001","11110"}},
        {'c', {"00000","00000","01110","10001","10000","10001","01110"}},
        {'d', {"00001","00001","01101","10011","10001","10001","01111"}},
        {'e', {"00000","00000","01110","10001","11111","10000","01110"}},
        {'f', {"00110","01001","01000","11100","01000","01000","01000"}},
        {'g', {"00000","00000","01111","10001","01111","00001","01110"}},
        {'h', {"10000","10000","10110","11001","10001","10001","10001"}},
        {'i', {"00100","00000","01100","00100","00100","00100","01110"}},
        {'j', {"00010","00000","00110","00010","00010","10010","01100"}},
        {'k', {"10000","10000","10010","10100","11000","10100","10010"}},
        {'l', {"01100","00100","00100","00100","00100","00100","01110"}},
        {'m', {"00000","00000","11010","10101","10101","10101","10101"}},
        {'n', {"00000","00000","10110","11001","10001","10001","10001"}},
        {'o', {"00000","00000","01110","10001","10001","10001","01110"}},
        {'p', {"00000","00000","11110","10001","11110","10000","10000"}},
        {'q', {"00000","00000","01101","10011","01111","00001","00001"}},
        {'r', {"00000","00000","10110","11001","10000","10000","10000"}},
        {'s', {"00000","00000","01111","10000","01110","00001","11110"}},
        {'t', {"01000","01000","11100","01000","01000","01001","00110"}},
        {'u', {"00000","00000","10001","10001","10001","10011","01101"}},
        {'v', {"00000","00000","10001","10001","10001","01010","00100"}},
        {'w', {"00000","00000","10001","10001","10101","10101","01010"}},
        {'x', {"00000","00000","10001","01010","00100","01010","10001"}},
        {'y', {"00000","00000","10001","10001","01111","00001","01110"}},
        {'z', {"00000","00000","11111","00010","00100","01000","11111"}}
    };

    auto it = font.find(c);
    if (it != font.end()) {
        return it->second;
    }

    return unknown;
}