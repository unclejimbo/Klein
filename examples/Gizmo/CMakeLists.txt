set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)

add_executable(Gizmo Main.cpp MainWindow.h MainWindow.cpp)

target_compile_features(Gizmo PRIVATE cxx_std_14)

target_link_libraries(Gizmo Klein)

set_target_properties(Gizmo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
