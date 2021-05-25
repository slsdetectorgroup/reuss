
set(PublicHeaders
    include/reuss/DetectorInterface.h
    include/reuss/JungfrauDetector.h
)

set(SourceFiles
  src/DummyDetector.cpp
  src/JungfrauDetector.cpp
)

add_library(
    reuss_detector SHARED ${SourceFiles} 
)

target_link_libraries(reuss_detector 
    PRIVATE 
        reuss::settings 
        fmt::fmt
        Threads::Threads 
        zmq 
        slsDetectorStatic

)
target_include_directories(reuss_detector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
set_target_properties(reuss_detector PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_detector
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    POSITION_INDEPENDENT_CODE ON
    PUBLIC_HEADER "${PublicHeaders}"
)

add_library(reuss::detector ALIAS reuss_detector)
