set(PublicHeaders
    reuss/Gotthard2Receiver.h
)

set(SourceFiles
  src/Gotthard2Receiver.cpp
)

if(REUSS_BUILD_DETECTOR)
    list(APPEND SourceFiles
        src/StreamingReceiver.cpp
    )
    list(APPEND PublicHeaders
        reuss/StreamingReceiver.h
        reuss/FrameAccumulator.h
        reuss/pedestal.h
    )
endif()

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
        reuss_math

)

if(REUSS_BUILD_DETECTOR)
target_link_libraries(reuss_components_object 
    PRIVATE 
        reuss::detector

)
endif()
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

if(REUSS_BUILD_TESTS)
    set(TestSources
        src/Gotthard2Receiver.test.cpp
    )
    target_sources(tests PRIVATE src ${TestSources} )
    target_link_libraries(tests PRIVATE reuss_components_shared)
endif(REUSS_BUILD_TESTS)