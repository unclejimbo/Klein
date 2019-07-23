add_subdirectory(BasicWindow)
add_subdirectory(CameraControl)
add_subdirectory(ImGuiWindow)
add_subdirectory(PBR)
add_subdirectory(ShadowMapping)
add_subdirectory(VertexColor)
add_subdirectory(Wireframe)

## copy data to build bin
add_custom_target(Klein-examples-data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:BasicWindow>/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:BasicWindow>/data
)
