


set(SourceFiles
  src/Streamer.cpp
  src/Receiver.cpp 
  src/ImageFifo.cpp
  src/UdpSocket.cpp 
  src/ZmqSocket.cpp 
  src/utils.cpp 
  src/DirectWriter.cpp
  src/FwriteWriter.cpp
)

add_library(
    reuss_core STATIC ${SourceFiles} 
)

target_link_libraries(reuss_core PRIVATE project_settings fmt Threads::Threads zmq)

target_include_directories(reuss_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

set_target_properties(reuss_core PROPERTIES
    ARCHIVE_OUTPUT_NAME reuss_core
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)