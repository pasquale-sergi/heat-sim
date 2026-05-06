# 2D Heat Diffusion Simulation

A small C++ project that simulates heat diffusion on a 2D plate.

The project was built to practice modern C++ concepts, basic scientific computing, numerical methods, and clean software architecture.

## What it does

The simulation models a rectangular plate where heat spreads over time.

## Physical model

The simulation is based on the 2D heat equation:

    dT/dt = alpha * (d2T/dx2 + d2T/dy2)

Where:

- T is temperature
- alpha is thermal diffusivity
- dt is the time step
- dx is the spatial step

For a material, thermal diffusivity is computed as:

    alpha = k / (rho * cp)

Where:

- k = thermal conductivity
- rho = density
- cp = specific heat capacity

## Numerical method

The project uses a simple explicit finite-difference update:

    T_next = T + r * (up + down + left + right - 4*T)

with:

    r = alpha * dt / dx^2

For stability, the explicit 2D method requires approximately:

    r <= 0.25

## Architecture

Main components:

- Grid2D: stores the 2D temperature field using a contiguous std::vector<double>.
- Material: stores material properties and computes thermal diffusivity.
- TimeStepper: abstract interface for numerical time-stepping algorithms.
- ExplicitEulerStepper: concrete finite-difference implementation of the heat update.
- BoundaryCondition: abstract interface for boundaries and heat sources.
- FixedTopBoundary: keeps the top row at a fixed temperature.
- VerticalHeatSource: keeps a vertical column at a fixed temperature.
- HeatSimulation: orchestrates the simulation loop, applies boundaries, runs the stepper, and writes output.
- ResultWriter: abstract interface for output generation.
- ContactSheetWriter: collects simulation frames and generates an annotated PPM contact sheet.

## Output

The simulation writes a PPM contact sheet:

    contact_sheet.ppm

The image contains:

- simulation title
- material properties
- numerical parameters
- frame captions by step number
- heatmap frames over time

<img width="940" height="890" alt="image" src="https://github.com/user-attachments/assets/2c3831a7-1269-4680-911a-918b99666e85" />
