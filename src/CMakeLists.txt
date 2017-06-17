cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CONFIG})

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(CGAL REQUIRED)
find_package(CGAL REQUIRED COMPONENTS CGAL_Core)
find_package(Euclid REQUIRED)
#find_package(PythonLibs REQUIRED)

include(${CGAL_USE_FILE})

include_directories(${CGAL_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${Euclid_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIR})

add_definitions(-DQT_NO_KEYWORDS)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SHADERS ${CMAKE_SOURCE_DIR}/contents/shaders/*)
file(GLOB_RECURSE SCRIPTS ${CMAKE_SOURCE_DIR}/contents/scripts/*.py)
add_executable(Klein ${SOURCES} ${HEADERS} ${SHADERS} ${SCRIPTS})
source_group("Scripts" FILES ${SCRIPTS})
source_group("Shaders" FILES ${SHADERS})

qt5_use_modules(Klein Core Widgets Gui)
target_link_libraries(Klein ${CGAL_LIBRARIES} ${PYTHON_LIBRARIES})
