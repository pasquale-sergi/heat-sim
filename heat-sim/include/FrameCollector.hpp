//Store frames in memory instead of saving a contact sheet and then we use them for the comparison

#pragma once

#include <vector>
#include "PPMImage.hpp"
#include "ResultWriter.hpp"
#include "Grid2D.hpp"

class FrameCollector : public ResultWriter {
public:
	FrameCollector(double minValue, double maxValue);

	void write(const Grid2D& grid, int step) const override;

	const std::vector<PPMImage>& frames() const;
	const std::vector<int>& steps() const;

private:
	double minValue_;
	double maxValue_;

	mutable std::vector<PPMImage> frames_;
	mutable std::vector<int> steps_;

	PPMImage gridToImage(const Grid2D& grid) const;

	static int clampToByte(double value);
};
