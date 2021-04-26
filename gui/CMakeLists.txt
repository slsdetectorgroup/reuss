

# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)

# find_package(OpenCV REQUIRED)
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# link_directories( ${OpenCV_LIB_DIR} )
# find_package(TIFF REQUIRED)

# find_package(Qt5 COMPONENTS Widgets REQUIRED)

# add_executable(reuss
#     src/mainwindow.ui
#     src/mainwindow.cpp
#     src/main.cpp
#     src/ImagePlot.cpp
#     resources/resources.qrc
# )

# target_include_directories(reuss PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# target_link_libraries(reuss Qt5::Widgets project_settings reuss_core_shared ${OpenCV_LIBS} TIFF::TIFF)

# set_target_properties(reuss PROPERTIES
#     RUNTIME_OUTPUT_NAME reuss_gui
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )

