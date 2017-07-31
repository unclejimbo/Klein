cmake_minimum_required(VERSION 3.8)
project(Klein CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

set(Klein_VERSION_MAJOR 0)
set(Klein_VERSION_MINOR 1)
set(Klein_VERSION_PATCH 0)

configure_file(
	${PROJECT_SOURCE_DIR}/config/KleinConfig.h.in
	${PROJECT_BINARY_DIR}/KleinConfig.h
)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/)

add_subdirectory(src)
