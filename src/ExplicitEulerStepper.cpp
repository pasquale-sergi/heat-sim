#include "ExplicitEulerStepper.hpp"

void ExplicitEulerStepper::step(
    const Grid2D& current,
    Grid2D& next,
    double alpha,
    double dt,
    double dx
) const {
    const double r = alpha * dt / (dx * dx);

    for (int row = 0; row < current.height(); ++row) {
        for (int col = 0; col < current.width(); ++col) {

            const bool isBoundary =
                row == 0 ||
                row == current.height() - 1 ||
                col == 0 ||
                col == current.width() - 1;

            if (isBoundary) {
                next.set(row, col, current.get(row, col));
                continue;
            }

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