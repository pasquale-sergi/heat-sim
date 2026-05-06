cmake_minimum_required(VERSION 3.20)

project(HeatSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(heat_sim
    src/main.cpp
    src/Grid2D.cpp
    src/ExplicitEulerStepper.cpp
    src/HeatSimulation.cpp
    src/PpmImageWriter.cpp
    src/FixedTopBoundary.cpp
 "src/HeatSimulation.cpp" "src/PpmImageWriter.cpp" "src/FixedTopBoundary.cpp")

target_include_directories(heat_sim PRIVATE include)
target_include_directories(heat_sim PRIVATE heat-sim/include)

if (MSVC)
    target_compile_options(heat_sim PRIVATE /W4 /permissive-)
else()
    target_compile_options(heat_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()