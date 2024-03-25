
pybind11_add_module(_reuss
    src/calibration.cpp
    src/frameacc.cpp
    src/interface.cpp
    src/main.cpp 
    src/pedestal.cpp
    src/zmqwriter.cpp
    src/streaming_receiver.cpp
    src/utils.cpp
)

target_link_libraries(_reuss 
    PRIVATE 
        project_settings 
        reuss_core_shared 
        reuss_components_shared 
        reuss_detector 
        reuss_math 
        libzmq-static
)


set_target_properties(_reuss PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

#Copy python souce files
set(PythonFiles
    __init__.py
    calibration.py
    config.py
    DataCollector.py
    formatting.py
    io.py
    overlay.py
    print_config.py
    PyZmqReceiver.py
    StreamWriter.py
    Hdf5File.py
    shm.py
    tools.py
    reussrc.in
)

foreach(FILE ${PythonFiles})
    configure_file( reuss/${FILE}  
    ${CMAKE_BINARY_DIR}/reuss/${FILE})
endforeach(FILE ${PYTHON_FILES})

#Copy python "executables"
set(PythonApps
    acquire
    cbf_writer
    collect_pd
    collect_raw_pd
    play.py
    srecv
    stream_reader
    stream_viewer
    viewer
    writer
    zmq_recv 
    replay.py
)

foreach(FILE ${PythonApps})
    configure_file( app/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach(FILE ${PYTHON_FILES})
