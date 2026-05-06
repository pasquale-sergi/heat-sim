#pragma once

#include <vector>
#include <stdexcept>

class Grid2D {
public:
	//contructor
	Grid2D(int width, int height, double initialValue);

	double get(int row, int col) const;
	void set(int row, int col, double value);

	int width() const;
	int height() const;

private:
	int width_;
	int height_;
	std::vector<double> values_;

	int index(int row, int col) const;
	void checkBounds(int row, int col) const;
};
