set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt53DCore REQUIRED)
find_package(Qt53DLogic REQUIRED)
find_package(Qt53DRender REQUIRED)
find_package(Qt53DExtras REQUIRED)

add_library(Klein
    # add headers for automoc to work
    ${PROJECT_SOURCE_DIR}/include/Klein/Gui/AbstractQt3DWindow.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Input/TrackballCameraController.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/BufferData.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/CheckerboardTextureImage.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ConcretePointsRenderer.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ForwardRenderingEffect.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/InstancedMeshRenderer.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/MeshGeometry.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/OffscreenRenderTarget.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/PBRMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/PBRInstancedMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/PBRSolidWireframeMaterial.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/ResourceManager.h
    ${PROJECT_SOURCE_DIR}/include/Klein/Render/Transformation.h

    ${CMAKE_CURRENT_SOURCE_DIR}/Gui/AbstractQt3DWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Input/TrackballCameraController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/CheckerboardTextureImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/ConcretePointsRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/ForwardRenderingEffect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/InstancedMeshRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/MeshGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/OffscreenRenderTarget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/PBRMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/PBRInstancedMaterial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/PBRSolidWireframeMaterial.cpp
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
