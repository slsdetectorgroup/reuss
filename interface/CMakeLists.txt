
# find_package(slsDetectorPackage REQUIRED)
find_package(OpenCV REQUIRED)

set(PublicHeaders
    include/reuss/StreamingReceiver.h
)

set(SourceFiles
  src/StreamingReceiver.cpp
  src/Pedestal.cpp
)

add_library(
    reuss_interface_object OBJECT ${SourceFiles} 
)

target_link_libraries(reuss_interface_object 
    PRIVATE 
        project_settings 
        fmt 
        Threads::Threads 
        zmq 
        reuss_core_object 
        slsDetectorObject
        ${OpenCV_LIBS}

)
target_include_directories(reuss_interface_object PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
set_target_properties(reuss_interface_object PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_interface_object
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    POSITION_INDEPENDENT_CODE ON
    PUBLIC_HEADER "${PublicHeaders}"
)



#Shared library
add_library(reuss_interface_shared SHARED $<TARGET_OBJECTS:reuss_interface_object>)
target_link_libraries(reuss_interface_shared PUBLIC reuss_interface_object reuss_core_shared slsDetectorShared)
set_target_properties(reuss_interface_shared PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_interface
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

#Static library
add_library(reuss_interface_static STATIC $<TARGET_OBJECTS:reuss_interface_object>)
target_link_libraries(reuss_interface_static PUBLIC reuss_interface_object reuss_core_static slsDetectorStatic)
set_target_properties(reuss_interface_static PROPERTIES
    ARCHIVE_OUTPUT_NAME reuss_interface
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)