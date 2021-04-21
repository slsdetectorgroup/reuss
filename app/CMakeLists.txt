


add_executable(urecv urecv.cpp)

target_link_libraries(urecv PUBLIC reuss_core)

set_target_properties(urecv PROPERTIES
    RUNTIME_OUTPUT_NAME urecv
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

