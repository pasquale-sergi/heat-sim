#pragma once

#include <string>
#include <vector>

#include "ResultWriter.hpp"
#include "PPMImage.hpp"
#include "Material.hpp"

class ContactSheetWriter : public ResultWriter {
public:
    ContactSheetWriter(double minValue, double maxValue, int columns);

    void write(const Grid2D& grid, int step) const override;

    void setTitle(std::string title);
	void setMaterialInfo(const Material& material, double dt, double dx);

    void save(const std::string& filename) const;

private:
    double minValue_;
    double maxValue_;
    int columns_;

    std::string title_;
	std::vector<std::string> infoLines_;

    mutable std::vector<PPMImage> frames_;
	mutable std::vector<int> steps_;

    PPMImage gridToImage(const Grid2D& grid) const;
    static int clampToByte(double value);

	static void drawText(PPMImage& image, int x, int y, const std::string& text, Pixel color, int scale = 2);

	static void drawChar(PPMImage& image, int x, int y, char c, Pixel color, int scale);

    static const std::vector<std::string>& glyphFor(char c);
};