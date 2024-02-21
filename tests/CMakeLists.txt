include(CTest)
include(Catch)

set(TEST_SOURCES 
    # main.cpp
    # src/DataSpan.test.cpp
    # src/ImageData.test.cpp
    # src/utils.test.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain project_settings)
catch_discover_tests(tests)