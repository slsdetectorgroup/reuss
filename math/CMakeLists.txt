
set(PublicHeaders
    reuss/DataSpan.h
    reuss/ImageData.h
    reuss/file_io.h
)

# set(SourceFiles
#   src/file_io.cpp
# )

add_library(reuss_math INTERFACE)
target_include_directories(reuss_math INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(reuss_math INTERFACE fmt)

if(REUSS_BUILD_TESTS)
    set(TestSources
        src/ImageData.test.cpp
        src/DataSpan.test.cpp
        src/file_io.test.cpp
    )
    target_sources(tests PRIVATE src ${TestSources} )
    target_link_libraries(tests PRIVATE reuss_math reuss::settings)


    #Copy data file for unit tests
    set(TestDataFiles
        data/np_1d.npy
        data/np_2d.npy
    )

    foreach(FILE ${TestDataFiles})
        configure_file( ${FILE}  
        ${CMAKE_BINARY_DIR}/tests/${FILE}
        COPYONLY)
    endforeach(FILE ${PYTHON_FILES})
endif(REUSS_BUILD_TESTS)

# add_library(
#     reuss_math_object OBJECT ${SourceFiles} 
# )

# target_link_libraries(reuss_math_object PRIVATE project_settings fmt Threads::Threads)

# target_include_directories(reuss_math_object PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# set_target_properties(reuss_math_object PROPERTIES
#     LIBRARY_OUTPUT_NAME reuss_math_object
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     POSITION_INDEPENDENT_CODE ON
#     PUBLIC_HEADER "${PublicHeaders}"
# )



# #Shared library
# add_library(reuss_math_shared SHARED $<TARGET_OBJECTS:reuss_math_object>)
# target_link_libraries(reuss_math_shared PUBLIC reuss_math_object)
# set_target_properties(reuss_math_shared PROPERTIES
#     LIBRARY_OUTPUT_NAME reuss_math
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )

# #Static library
# add_library(reuss_math_static STATIC $<TARGET_OBJECTS:reuss_math_object>)
# target_link_libraries(reuss_math_static PUBLIC reuss_math_object)
# set_target_properties(reuss_math_static PROPERTIES
#     ARCHIVE_OUTPUT_NAME reuss_math
#     ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     # PUBLIC_HEADER "${PUBLICHEADERS}"
# )