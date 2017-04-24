cmake_minimum_required(VERSION 3.5)
project(Klein CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

set(Klein_VERSION_MAJOR 0)
set(Klein_VERSION_MINOR 1)
set(Klein_VERSION_PATCH 0)

message(STATUS ${PROJECT_SOURCE_DIR})
message(STATUS ${PROJECT_BINARY_DIR})
configure_file(
	${PROJECT_SOURCE_DIR}/config/KleinConfig.h.in
	${PROJECT_BINARY_DIR}/KleinConfig.h
)
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)
