
pybind11_add_module(_reuss
    src/main.cpp 
    src/zmqwriter.cpp
    src/pedestal.cpp
    src/interface.cpp
)

target_link_libraries(_reuss PRIVATE project_settings reuss_core_shared reuss_interface_shared reuss_math)



set_target_properties(_reuss PROPERTIES
                        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                    )

#Copy python souce files
set(PythonFiles
    __init__.py
    config.py
    formatting.py
    DataCollector.py
)

foreach(FILE ${PythonFiles})
    configure_file( reuss/${FILE}  
    ${CMAKE_BINARY_DIR}/reuss/${FILE})
endforeach(FILE ${PYTHON_FILES})

#Copy python "executables"
set(PythonApps
    writer
    zmq_recv
    collect_pd
)

foreach(FILE ${PythonApps})
    configure_file( app/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach(FILE ${PYTHON_FILES})
