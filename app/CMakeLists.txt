


add_executable(sender sender.cpp)
target_link_libraries(sender PUBLIC reuss_core_static project_settings fmt::fmt)
set_target_properties(sender PROPERTIES
    RUNTIME_OUTPUT_NAME sender
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

add_executable(take_pd take_pd.cpp)
target_link_libraries(take_pd PUBLIC reuss_core_static reuss_detector reuss_math project_settings reuss_components_shared fmt::fmt)
set_target_properties(take_pd PROPERTIES
    RUNTIME_OUTPUT_NAME take_pd
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)


add_executable(urecv urecv.cpp)
target_link_libraries(urecv PUBLIC reuss_core_static project_settings reuss_detector reuss_components_shared fmt::fmt)
set_target_properties(urecv PROPERTIES
    RUNTIME_OUTPUT_NAME urecv
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # PUBLIC_HEADER "${PUBLICHEADERS}"
)

# add_executable(srecv srecv.cpp)
# target_link_libraries(srecv PUBLIC reuss_core_static reuss_math project_settings reuss_detector reuss_components_shared fmt::fmt)
# set_target_properties(srecv PROPERTIES
#     RUNTIME_OUTPUT_NAME cppsrecv
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )

# add_executable(benchmark_sum benchmark_sum.cpp)
# target_link_libraries(benchmark_sum PUBLIC reuss_core_static reuss_math project_settings reuss_detector reuss_components_shared fmt::fmt)
# set_target_properties(benchmark_sum PROPERTIES
#     RUNTIME_OUTPUT_NAME benchmark_sum
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )



#Copy extra files to the binary dir
set(Extras
    em_virtual.config
)
foreach(FILE ${Extras})
    configure_file( ${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach(FILE ${PYTHON_FILES})
