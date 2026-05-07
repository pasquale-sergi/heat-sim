# 2D Heat Diffusion Simulation

A small scientific-computing project that simulates heat diffusion on a 2D plate and exports the result as PPM images.

The project was built to practice modern C++, numerical methods, clean software architecture, and simple scientific visualization.

## Overview

The simulation represents a rectangular plate as a 2D grid of temperature values. At each time step, every internal cell is updated according to the temperature of its four direct neighbors: up, down, left, and right.

The program can:

- simulate heat diffusion for different materials;
- generate an annotated PPM contact sheet for a single material;
- compare two materials side by side;
- visualize how material properties affect heat spreading over time.

## Physical model

The simulation is based on the 2D heat equation:

```text
∂T/∂t = α(∂²T/∂x² + ∂²T/∂y²)
```

Where:

- `T` is temperature;
- `α` is thermal diffusivity;
- `t` is time;
- `x` and `y` are spatial coordinates.

For each material, thermal diffusivity is computed as:

```text
α = k / (ρ cp)
```

Where:

- `k` is thermal conductivity, in W/(m·K);
- `ρ` is density, in kg/m³;
- `cp` is specific heat capacity, in J/(kg·K).

Higher thermal diffusivity means heat spreads faster through the material.

## Numerical method

The project uses an explicit finite-difference method with an Explicit Euler time stepper.

For every internal grid cell, the update formula is:

```text
T_next = T_current + r(up + down + left + right - 4T_current)
```

with:

```text
r = α dt / dx²
```

Where:

- `dt` is the time step;
- `dx` is the spatial step;
- `up`, `down`, `left`, and `right` are the neighboring cell temperatures.

## Supported materials

The project currently supports:

- Aluminum
- Copper
- Steel
- Ceramic

Each material stores:

- thermal conductivity;
- density;
- specific heat capacity;
- computed thermal diffusivity.

The material system allows the same simulation logic to be reused with different physical parameters.

## Output files

The generated PPM images include multiple simulation frames over time. The contact sheet also includes metadata such as the material name, physical parameters, and numerical parameters.

### Single material
<img width="890" height="596" alt="Screenshot 2026-05-07 110152" src="https://github.com/user-attachments/assets/b6e5def5-7b52-4489-a0bb-9db652b6a24b" />

### Comparison between two materials
<img width="744" height="1005" alt="Screenshot 2026-05-07 110332" src="https://github.com/user-attachments/assets/a66851a2-27f9-4dce-b15e-c5ba53d85203" />

