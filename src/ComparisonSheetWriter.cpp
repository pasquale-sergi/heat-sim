#include "ComparisonSheetWriter.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

void ComparisonSheetWriter::save(
    const Material& firstMaterial,
    const FrameCollector& firstFrames,
    const Material& secondMaterial,
    const FrameCollector& secondFrames,
    double dt,
    const std::string& filename
) const {
    const auto& leftFrames = firstFrames.frames();
    const auto& rightFrames = secondFrames.frames();
    const auto& steps = firstFrames.steps();

    if (leftFrames.empty() || rightFrames.empty()) {
        throw std::runtime_error("Cannot create comparison sheet: missing frames.");
    }

    if (leftFrames.size() != rightFrames.size()) {
        throw std::runtime_error("Both materials must have the same number of frames.");
    }

    if (firstFrames.steps() != secondFrames.steps()) {
        throw std::runtime_error("Both materials must have matching step numbers.");
    }

    const int frameWidth = leftFrames[0].width();
    const int frameHeight = leftFrames[0].height();

    const int titleHeight = 90;
    const int labelWidth = 130;
    const int columnGap = 20;
    const int rowGap = 35;
    const int margin = 15;

    const int sheetWidth =
        margin +
        labelWidth +
        frameWidth +
        columnGap +
        frameWidth +
        margin;

    const int sheetHeight =
        titleHeight +
        static_cast<int>(leftFrames.size()) * (frameHeight + rowGap) +
        margin;

    PPMImage sheet(sheetWidth, sheetHeight);
    fill(sheet, Pixel{ 255, 255, 255 });

    const Pixel black{ 0, 0, 0 };

    const std::string title =
        firstMaterial.name() + " vs " + secondMaterial.name();

    drawText(sheet, margin, 15, title, black, 3);

    drawText(
        sheet,
        margin + labelWidth,
        60,
        firstMaterial.name(),
        black,
        2
    );

    drawText(
        sheet,
        margin + labelWidth + frameWidth + columnGap,
        60,
        secondMaterial.name(),
        black,
        2
    );

    for (int i = 0; i < static_cast<int>(leftFrames.size()); ++i) {
        const int y = titleHeight + i * (frameHeight + rowGap);

        const int leftX = margin + labelWidth;
        const int rightX = leftX + frameWidth + columnGap;

        std::ostringstream label;
        label << "Step " << steps[i];

        drawText(sheet, margin, y + frameHeight / 2 - 8, label.str(), black, 1);

        std::ostringstream timeLabel;
        timeLabel << "t=" << steps[i] * dt << "s";

        drawText(sheet, margin, y + frameHeight / 2 + 10, timeLabel.str(), black, 1);

        copyImage(leftFrames[i], sheet, leftX, y);
        copyImage(rightFrames[i], sheet, rightX, y);
    }

    std::ofstream file(filename);

    if (!file) {
        throw std::runtime_error("Could not open comparison sheet output file.");
    }

    file << "P3\n";
    file << sheet.width() << " " << sheet.height() << "\n";
    file << "255\n";

    for (int y = 0; y < sheet.height(); ++y) {
        for (int x = 0; x < sheet.width(); ++x) {
            Pixel pixel = sheet.getPixel(x, y);

            file << pixel.r << " "
                << pixel.g << " "
                << pixel.b << " ";
        }

        file << "\n";
    }
}

void ComparisonSheetWriter::copyImage(
    const PPMImage& source,
    PPMImage& destination,
    int offsetX,
    int offsetY
) {
    for (int y = 0; y < source.height(); ++y) {
        for (int x = 0; x < source.width(); ++x) {
            destination.setPixel(
                offsetX + x,
                offsetY + y,
                source.getPixel(x, y)
            );
        }
    }
}

void ComparisonSheetWriter::fill(PPMImage& image, Pixel color) {
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            image.setPixel(x, y, color);
        }
    }
}

void ComparisonSheetWriter::drawText(
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

void ComparisonSheetWriter::drawChar(
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

const std::vector<std::string>& ComparisonSheetWriter::glyphFor(char c) {
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
        {'.', {"00000","00000","00000","00000","00000","00100","00100"}},
        {':', {"00000","00100","00100","00000","00100","00100","00000"}},
        {'=', {"00000","11111","00000","11111","00000","00000","00000"}},
        {'-', {"00000","00000","00000","11111","00000","00000","00000"}},

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