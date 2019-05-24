set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets 3DCore 3DLogic 3DRender 3DExtras)

add_library(Klein
    # add headers for automoc to work
    ${PROJECT_SOURCE_DIR}/include/Klein/Gui/AbstractQt3DWindow.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Gui/ImGuiManager.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Gui/ImGuiOutputWindow.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Input/TrackballCameraController.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/BasePBRMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/BaseUnlitMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/BufferData.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/CheckerboardTextureImage.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ColorRenderTarget.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ConcreteLinesRenderer.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ConcretePointsRenderer.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/DirectionalShadowCaster.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/InstancedMeshRenderer.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/MeshGeometry.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/PBRMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/PBRInstancedMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/PBRSolidWireframeMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ResourceManager.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/Transformation.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/UnlitMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/UnlitInstancedMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/UnlitSolidWireframeMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Util/ColorPalette.h

    ${CMAKE_CURRENT_SOURCE_DIR}/Gui/AbstractQt3DWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Gui/ImGuiManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Gui/ImGuiOutputWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Input/TrackballCameraController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/BasePBRMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/BaseUnlitMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/CheckerboardTextureImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/ColorRenderTarget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/ConcreteLinesRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/ConcretePointsRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/DirectionalShadowCaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/InstancedMeshRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/MeshGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/PBRMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/PBRInstancedMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/PBRSolidWireframeMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/UnlitMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/UnlitInstancedMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/UnlitSolidWireframeMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/ColorPalette.cpp

    ${PROJECT_SOURCE_DIR}/external/imgui/src/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/src/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/src/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/src/imgui.cpp
)
add_library(Klein::Klein ALIAS Klein)

option(BUILD_SHARED_LIBS "Build shared library" ON)
include(GenerateExportHeader)
generate_export_header(Klein
    EXPORT_MACRO_NAME KLEIN_API
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/Klein/Core/Export.h
)

target_compile_features(Klein PUBLIC cxx_std_14)

target_include_directories(Klein
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/imgui/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(Klein
    PUBLIC
        Qt5::Core
        Qt5::Widgets
        Qt5::3DCore
        Qt5::3DLogic
        Qt5::3DRender
        Qt5::3DExtras
)

set_target_properties(Klein PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Klein)

install(TARGETS Klein
    EXPORT Klein-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/Klein
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/Klein
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/data
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT Klein-targets
    FILE KleinTargets.cmake
    NAMESPACE Klein::
    DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/KleinConfigVersion.cmake
    VERSION ${Klein_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/KleinConfig.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/KleinConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/cmake/KleinConfig.cmake
        ${CMAKE_BINARY_DIR}/cmake/KleinConfigVersion.cmake
    DESTINATION
        ${INSTALL_CONFIGDIR}
)

export(EXPORT Klein-targets
    FILE ${CMAKE_BINARY_DIR}/cmake/KleinTargets.cmake
    NAMESPACE Klein::
)

export(PACKAGE Klein)
