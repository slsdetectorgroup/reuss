

# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)

# find_package(OpenCV REQUIRED)
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# link_directories( ${OpenCV_LIB_DIR} )
# find_package(TIFF REQUIRED)

# find_package(Qt5 COMPONENTS Widgets REQUIRED)

set( IMGUI_SOURCES
 libs/imgui

)

add_executable(reuss_gui
    app/reuss_gui.cpp
)

# target_include_directories(reuss PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(reuss_gui project_settings reuss_core_shared ImGui-SFML::ImGui-SFML)

set_target_properties(reuss_gui PROPERTIES
    RUNTIME_OUTPUT_NAME reuss_gui
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

