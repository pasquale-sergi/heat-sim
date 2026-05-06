#pragma once

#include "Grid2D.hpp"

class ResultWriter {
public:
	virtual ~ResultWriter() = default;

	virtual void write(const Grid2D& grid, int step) const = 0;
};