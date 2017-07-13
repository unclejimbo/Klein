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
find_package(OpenCV REQUIRED core imgproc)

set(WITH_OPENMP TRUE CACHE BOOL "Enable OpenMP")
if (WITH_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

include(${CGAL_USE_FILE})

include_directories(${CGAL_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${Euclid_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_definitions(-DQT_NO_KEYWORDS)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SHADERS ${CMAKE_SOURCE_DIR}/contents/shaders/*)
add_executable(Klein ${SOURCES} ${HEADERS} ${SHADERS})
source_group("Shaders" FILES ${SHADERS})

qt5_use_modules(Klein Core Widgets Gui)
target_link_libraries(Klein ${CGAL_LIBRARIES} ${OpenCV_LIBS})
