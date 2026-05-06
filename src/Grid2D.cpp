#include "Grid2D.hpp"

Grid2D::Grid2D(int width, int height, double initialValue) : width_(width), height_(height), values_(width * height, initialValue) {
	if (width <= 0 || height <= 0) {
		throw std::invalid_argument("Grid dimension must be positive");
	}
}

double Grid2D::get(int row, int col) const {
	checkBounds(row, col);
	return values_[index(row, col)];
}

void Grid2D::set(int row, int col, double value) {
	checkBounds(row, col);
	values_[index(row, col)] = value;
}

int Grid2D::width() const {
	return width_;
}

int Grid2D::height() const {
	return height_;
}

int Grid2D::index(int row, int col) const {
	return row * width_ + col;
}

void Grid2D::checkBounds(int row, int col) const {
	if (row < 0 || row >= height_ || col < 0 || col >= width_) {
		throw std::out_of_range("Grid index out of range.");
	}
}