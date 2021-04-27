

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
    src/table.cpp
)
find_package(OpenCV REQUIRED)
target_include_directories(reuss_gui PRIVATE src)

target_link_libraries(reuss_gui project_settings reuss_core_shared ImGui-SFML::ImGui-SFML ${OpenCV_LIBS})

set_target_properties(reuss_gui PROPERTIES
    RUNTIME_OUTPUT_NAME reuss_gui
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

