# Only sources that don't depend on the 
# detector iterface for portability
set(PythonCppSources
    src/main.cpp 
    src/zmqwriter.cpp
    src/calibration.cpp
    src/utils.cpp
    src/g2receiver.cpp
)


if(REUSS_BUILD_DETECTOR)
    list(APPEND PythonCppSources
        src/pedestal.cpp
        src/interface.cpp
        src/frameacc.cpp
        src/streaming_receiver.cpp
    )
endif()

pybind11_add_module(_reuss
    ${PythonCppSources}
)

target_link_libraries(_reuss PRIVATE project_settings reuss_core_shared reuss_components_shared  reuss_math)

if(REUSS_BUILD_DETECTOR)
    target_link_libraries(_reuss PRIVATE reuss_detector)
endif()

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
    validation.py
    overlay.py
    tools.py
    DataCollector.py
    DummyDetector.py
    PreviewReceiver.py
    reussrc.in
    receiver.py
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
    zmq_recv
    collect_pd
    acquire
    collect_raw_pd
    play.py
    read_stream.py
    g2panel
    g2panel.ui
    g2recv
)

foreach(FILE ${PythonApps})
    configure_file( app/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach(FILE ${PYTHON_FILES})

#Python test files 
set(PythonTests
    test_load_channel_mask.py
    channel_mask.txt
)

foreach(FILE ${PythonTests})
    configure_file( tests/${FILE}  
    ${CMAKE_BINARY_DIR}/python_tests/${FILE})
endforeach(FILE ${PYTHON_FILES})