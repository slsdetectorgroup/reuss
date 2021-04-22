


add_executable(urecv urecv.cpp)

target_link_libraries(urecv PUBLIC reuss_core_static project_settings)

set_target_properties(urecv PROPERTIES
    RUNTIME_OUTPUT_NAME urecv
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)


add_executable(sender sender.cpp)

target_link_libraries(sender PUBLIC reuss_core_static project_settings)

set_target_properties(sender PROPERTIES
    RUNTIME_OUTPUT_NAME sender
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

