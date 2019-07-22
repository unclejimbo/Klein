set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)

add_executable(Wireframe Main.cpp MainWindow.h MainWindow.cpp)

target_compile_features(Wireframe PRIVATE cxx_std_14)

target_link_libraries(Wireframe Klein)

set_target_properties(Wireframe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
