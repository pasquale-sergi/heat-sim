#include "VerticalHeatSource.hpp"
#include <stdexcept>

VerticalHeatSource::VerticalHeatSource(int col, double temperature)
	: col_(col), temperature_(temperature) {

	//check values are valid
	if (col < 0) {
		throw std::invalid_argument("Column index cannot be negative.");
	}
}

void VerticalHeatSource::apply(Grid2D& grid) const {
	if (col_ >= grid.width()) {
		throw std::out_of_range("Column index exceeds grid width.");
	}
	//populate the private data of the VerticalHeat class
	for (int row = 0; row < grid.height(); ++row) {
		grid.set(row, col_, temperature_);
	}
}

