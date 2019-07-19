cmake_minimum_required(VERSION 3.12)

project(KLEIN
    LANGUAGES CXX
    VERSION 0.2.0
)

add_subdirectory(src)

option(BUILD_EXAMPLES "Build examples" OFF)
if (BUILD_EXAMPLES)
add_subdirectory(examples)
endif()
