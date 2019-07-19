set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)

add_executable(BasicWindow Main.cpp MainWindow.h MainWindow.cpp)

target_compile_features(BasicWindow PRIVATE cxx_std_14)

target_link_libraries(BasicWindow Klein)

set_target_properties(BasicWindow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
