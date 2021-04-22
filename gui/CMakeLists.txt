

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(reuss
    src/mainwindow.ui
    src/mainwindow.cpp
    src/main.cpp
    resources/resources.qrc
)

target_link_libraries(reuss Qt5::Widgets project_settings reuss_core_shared)

set_target_properties(reuss PROPERTIES
    RUNTIME_OUTPUT_NAME reuss_gui
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)