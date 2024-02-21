set(PublicHeaders
    include/reuss/ZmqWriter.h
)

set(SourceFiles
  src/Streamer.cpp
  src/Receiver.cpp 
  src/ImageFifo.cpp
  src/UdpSocket.cpp 
  src/ZmqSocket.cpp 
  src/utils.cpp 
  src/DirectWriter.cpp
  src/FwriteWriter.cpp
  src/ZmqReceiver.cpp
  src/ZmqWriter.cpp
)

add_library(
    reuss_core_object OBJECT ${SourceFiles} 
)

target_link_libraries(reuss_core_object PRIVATE reuss::settings reuss_math fmt::fmt Threads::Threads libzmq)
target_include_directories(reuss_core_object PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(reuss_core_object PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_core_object
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    POSITION_INDEPENDENT_CODE ON
    PUBLIC_HEADER "${PublicHeaders}"
)


#Shared library
add_library(reuss_core_shared SHARED $<TARGET_OBJECTS:reuss_core_object>)
target_link_libraries(reuss_core_shared PUBLIC reuss_core_object)
set_target_properties(reuss_core_shared PROPERTIES
    LIBRARY_OUTPUT_NAME reuss_core
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

#Static library
add_library(reuss_core_static STATIC $<TARGET_OBJECTS:reuss_core_object>)
target_link_libraries(reuss_core_static PUBLIC reuss_core_object)
set_target_properties(reuss_core_static PROPERTIES
    ARCHIVE_OUTPUT_NAME reuss_core
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

add_library(reuss::core_static ALIAS reuss_core_static)
add_library(reuss::core_shared ALIAS reuss_core_shared)

if(REUSS_BUILD_TESTS)
    set(TestSources
        src/StaticString.test.cpp
        src/SimpleQueue.test.cpp
        src/ImageFifo.test.cpp
    )
    target_sources(tests PRIVATE src ${TestSources} )
    target_link_libraries(tests PRIVATE reuss::core_shared)
endif(REUSS_BUILD_TESTS)
