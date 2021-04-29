
# set(IMGUI_SOURCES
#     ${PROJECT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
#     ${PROJECT_SOURCE_DIR}/libs/imgui/imgui_demo.cpp
#     ${PROJECT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
#     ${PROJECT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
#     ${PROJECT_SOURCE_DIR}/libs/imgui/imgui.cpp
#     ${PROJECT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl3.cpp
#     ${PROJECT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_glfw.cpp 
   
# )

# add_executable(reuss_gui
#     app/reuss_gui_opengl.cpp
#     src/table.cpp
#     ${IMGUI_SOURCES}

# )

# find_package(OpenGL 4.6 REQUIRED)
# find_package(GLEW REQUIRED)
# find_package(OpenCV REQUIRED)
# target_include_directories(reuss_gui PRIVATE src ${PROJECT_SOURCE_DIR}/libs/imgui)

# target_link_libraries(reuss_gui project_settings reuss_core_shared  glfw OpenGL::GL ${OpenCV_LIBS} ${GLEW_LIBRARIES})

# set_target_properties(reuss_gui PROPERTIES
#     RUNTIME_OUTPUT_NAME reuss_gui
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )



add_executable(reuss_gui
    app/reuss_gui.cpp
    src/table.cpp
)
target_compile_features(reuss_gui PRIVATE cxx_std_17 )

find_package(OpenGL 4.6 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV REQUIRED)
# find_package(ImGui)
target_include_directories(reuss_gui PRIVATE src ${PROJECT_SOURCE_DIR}/libs/imgui)

target_link_libraries(reuss_gui  reuss_core_shared  glfw OpenGL::GL ${OpenCV_LIBS} ${GLEW_LIBRARIES} ImGui-SFML::ImGui-SFML)

set_target_properties(reuss_gui PROPERTIES
    RUNTIME_OUTPUT_NAME reuss_gui
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)


# IMGUI_IMPL_OPENGL_LOADER_GL3W