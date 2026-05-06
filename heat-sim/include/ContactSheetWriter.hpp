#pragma once

#include <string>
#include <vector>

#include "ResultWriter.hpp"
#include "PPMImage.hpp"

class ContactSheetWriter : public ResultWriter {
public:
    ContactSheetWriter(double minValue, double maxValue, int columns);

    void write(const Grid2D& grid, int step) const override;

    void save(const std::string& filename) const;

private:
    double minValue_;
    double maxValue_;
    int columns_;

    mutable std::vector<PPMImage> frames_;

    PPMImage gridToImage(const Grid2D& grid) const;
    static int clampToByte(double value);
};