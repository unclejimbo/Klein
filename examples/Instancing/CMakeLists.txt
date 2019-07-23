set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core)

add_executable(Instancing Main.cpp MainWindow.h MainWindow.cpp ../external/tiny_obj_loader.cc)

target_compile_features(Instancing PRIVATE cxx_std_14)

target_link_libraries(Instancing Klein)

set_target_properties(Instancing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
