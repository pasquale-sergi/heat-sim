#pragma once

#include <string>

#include "FrameCollector.hpp"
#include "Material.hpp"
#include "PPMImage.hpp"

class ComparisonSheetWriter {
public:
    void save(
        const Material& firstMaterial,
        const FrameCollector& firstFrames,
        const Material& secondMaterial,
        const FrameCollector& secondFrames,
        double dt,
        const std::string& filename
    ) const;

private:
    static void copyImage(
        const PPMImage& source,
        PPMImage& destination,
        int offsetX,
        int offsetY
    );

    static void fill(
        PPMImage& image,
        Pixel color
    );

    static void drawText(
        PPMImage& image,
        int x,
        int y,
        const std::string& text,
        Pixel color,
        int scale = 2
    );

    static void drawChar(
        PPMImage& image,
        int x,
        int y,
        char c,
        Pixel color,
        int scale
    );

    static const std::vector<std::string>& glyphFor(char c);
};