cmake_minimum_required(VERSION 3.14)
project(reuss)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

#Force Release if build is not specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()


# option(RX_USE_TESTS "Build tests" ON)
option(REUSS_USE_SANITIZER "Sanitizers for debugging" OFF)
option(REUSS_TUNE_LOCAL "Tune to the specific CPU" OFF)
if(REUSS_USE_SANITIZER)
  # set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
  # set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")
  set(CMAKE_BUILD_TYPE "Debug")
endif(REUSS_USE_SANITIZER)

set(ClangFormat_EXCLUDE_PATTERNS  "build/" 
                                  "python/"
                                  ${CMAKE_BINARY_DIR})


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(ClangFormat)
find_package(fmt REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



add_library(project_settings INTERFACE)
target_compile_features(project_settings INTERFACE cxx_std_17)
target_compile_options(project_settings INTERFACE 
                            -Wall
                            -Wextra
                            # -Wshadow
                            -Wunused
                            -Wno-unused-parameter
                            -Wold-style-cast
                            -Wcast-align
                            -Wnon-virtual-dtor
                            -Woverloaded-virtual
                            -Wdouble-promotion
                            -Wformat=2
                            -Wredundant-decls
                            # -Wconversion
                            -Wnull-dereference
                            -Wdouble-promotion
                            -Werror=return-type
                            -Wpedantic
)
if(REUSS_TUNE_LOCAL)
    target_compile_options(project_settings INTERFACE -mtune=native -march=native)
endif()

add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(python)

# SET(source_list  "${source_list}" CACHE INTERNAL "source_list")
# set(<variable> <value>... CACHE <type> <docstring> [FORCE])
# set(<variable> <value>... CACHE <type> <docstring> [FORCE])
# set(IMGUI_DIR "libs/imgui" CACHE PATH)
set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui" CACHE PATH "Imgui top level dir")
set(IMGUI_SFML_FIND_SFML false CACHE BOOL "Look for sfml?")
# IMGUI_SFML_FIND_SFML
add_subdirectory(libs/SFML)
add_subdirectory(libs/imgui-sfml)


add_subdirectory(gui)

# add_executable(urecv app/urecv.cpp)
# target_link_libraries(urecv PUBLIC rxStatic)

# add_executable(write_test app/write_test.cpp)
# target_link_libraries(write_test PUBLIC rxStatic)

# set( PYTHON_FILES
#     read_binary.py
#     read_raw.py
#     zmqtest.py
# )
# foreach(FILE ${PYTHON_FILES})
#     configure_file( python/${FILE}  
#     ${CMAKE_BINARY_DIR}/${FILE}  )
# endforeach(FILE ${PYTHON_FILES})


# if(RX_USE_TESTS)
#   add_subdirectory(tests)
# endif(RX_USE_TESTS)