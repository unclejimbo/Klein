set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)

add_executable(PBR Main.cpp MainWindow.h MainWindow.cpp)

target_compile_features(PBR PRIVATE cxx_std_14)

target_link_libraries(PBR Klein)

set_target_properties(PBR PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
