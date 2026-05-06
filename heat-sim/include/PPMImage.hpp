#pragma once

#include <vector>
#include <stdexcept>
#include "Pixel.hpp"

class PPMImage {
public:
	//constructor
	PPMImage (int width, int height) : width_(width), height_(height), pixels_(width * height) {
		if (width <= 0 || height <= 0) {
			throw std::invalid_argument("Width and height must be positive integers.");
		}
	}

	int width() const {
		return width_;
	}

	int height() const {
		return height_;
	}

	Pixel getPixel(int x, int y) const {
		return pixels_.at(y * width_ + x);
	}

	void setPixel(int x, int y, Pixel pixel) {
		pixels_.at(y * width_ + x) = pixel;
	}

private:
	int width_;
	int height_;
	std::vector<Pixel> pixels_;
};