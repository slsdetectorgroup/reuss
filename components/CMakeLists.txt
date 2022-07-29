set(PublicHeaders
    reuss/StreamingReceiver.h
    reuss/Gotthard2Receiver.h
    reuss/FrameAccumulator.h
    reuss/pedestal.h
)

set(SourceFiles
  src/StreamingReceiver.cpp
  src/Gotthard2Receiver.cpp
)

add_library(
    reuss_components_object OBJECT ${SourceFiles} 
)

target_link_libraries(reuss_components_object 
    PRIVATE 
        project_settings 
        fmt::fmt
        Threads::Threads 
        zmq 
        reuss_core_object 
        reuss::detector
        reuss_math

)
target_include_directories(reuss_components_object PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
set_target_properties(reuss_components_object PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_components_object
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    POSITION_INDEPENDENT_CODE ON
    PUBLIC_HEADER "${PublicHeaders}"
)



#Shared library
add_library(reuss_components_shared SHARED $<TARGET_OBJECTS:reuss_components_object>)
target_link_libraries(reuss_components_shared PUBLIC reuss_components_object reuss_core_shared  )
set_target_properties(reuss_components_shared PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_components
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    PUBLIC_HEADER "${PublicHeaders}"
)

#Static library
add_library(reuss_components_static STATIC $<TARGET_OBJECTS:reuss_components_object>)
target_link_libraries(reuss_components_static PUBLIC reuss_components_object reuss_core_static  )
set_target_properties(reuss_components_static PROPERTIES
    ARCHIVE_OUTPUT_NAME reuss_components
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    PUBLIC_HEADER "${PublicHeaders}"
)