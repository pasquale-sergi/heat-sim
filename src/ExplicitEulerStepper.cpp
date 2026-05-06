#include "ExplicitEulerStepper.hpp"

#include <stdexcept>

void ExplicitEulerStepper::step(
    const Grid2D& current,
    Grid2D& next,
    double alpha,
    double dt,
    double dx
) const {
    const double r = alpha * dt / (dx * dx);

    if (r > 0.25) {
        throw std::invalid_argument("Unstable parameters: r must be <= 0.25 for this explicit 2D method.");
    }

    for (int row = 0; row < current.height(); ++row) {
        for (int col = 0; col < current.width(); ++col) {
            next.set(row, col, current.get(row, col));
        }
    }

    for (int row = 1; row < current.height() - 1; ++row) {
        for (int col = 1; col < current.width() - 1; ++col) {
            const double center = current.get(row, col);
            const double up = current.get(row - 1, col);
            const double down = current.get(row + 1, col);
            const double left = current.get(row, col - 1);
            const double right = current.get(row, col + 1);

            const double newValue =
                center + r * (up + down + left + right - 4.0 * center);

            next.set(row, col, newValue);
        }
    }
}   