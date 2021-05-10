



add_executable(sender sender.cpp)

target_link_libraries(sender PUBLIC reuss_core_static project_settings)

set_target_properties(sender PROPERTIES
    RUNTIME_OUTPUT_NAME sender
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

# add_executable(imshow imshow.cpp)

# target_link_libraries(imshow PUBLIC reuss_core_static project_settings)

# set_target_properties(imshow PROPERTIES
#     RUNTIME_OUTPUT_NAME imshow
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )

add_executable(urecv urecv.cpp)

target_link_libraries(urecv PUBLIC reuss_core_static project_settings reuss_interface_shared)

set_target_properties(urecv PROPERTIES
    RUNTIME_OUTPUT_NAME urecv
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

add_executable(demo demo.cpp)

target_link_libraries(demo PUBLIC reuss_core_static project_settings reuss_interface_shared reuss_math)

set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_NAME demo
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

#Copy python "executables"
set(Extras
    em_virtual.config
)

foreach(FILE ${Extras})
    configure_file( ${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach(FILE ${PYTHON_FILES})