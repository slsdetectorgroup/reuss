
pybind11_add_module(_reuss
    src/main.cpp 
    src/zmqwriter.cpp
    src/pedestal.cpp
    src/interface.cpp
    src/calibration.cpp
    src/utils.cpp
    src/frameacc.cpp
    src/streaming_receiver.cpp
)

target_link_libraries(_reuss PRIVATE project_settings reuss_core_shared reuss_components_shared reuss_detector reuss_math libzmq)



set_target_properties(_reuss PROPERTIES
                        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                    )

#Copy python souce files
set(PythonFiles
    __init__.py
    config.py
    io.py
    formatting.py
    calibration.py
    overlay.py
    tools.py
    DataCollector.py
    reussrc.in
    shm.py
    print_config.py
)

foreach(FILE ${PythonFiles})
    configure_file( reuss/${FILE}  
    ${CMAKE_BINARY_DIR}/reuss/${FILE})
endforeach(FILE ${PYTHON_FILES})

#Copy python "executables"
set(PythonApps
    writer
    viewer
    srecv
    zmq_recv
    collect_pd
    acquire
    collect_raw_pd
    play.py
)

foreach(FILE ${PythonApps})
    configure_file( app/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach(FILE ${PYTHON_FILES})
