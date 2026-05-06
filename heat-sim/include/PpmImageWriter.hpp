#pragma once

#include <string>
#include "ResultWriter.hpp"

class PpmImageWriter : public ResultWriter {
public:
    PpmImageWriter(std::string outputDirectory, double minValue, double maxValue);

    void write(const Grid2D& grid, int step) const override;

private:
    std::string outputDirectory_;
    double minValue_;
    double maxValue_;

    static int clampToByte(double value);
};