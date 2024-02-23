cmake_minimum_required(VERSION 3.17)
project(reuss)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

if (${CMAKE_VERSION} VERSION_GREATER "3.24")
    cmake_policy(SET CMP0135 NEW) #Fetch content download timestamp
endif()
include(FetchContent)

#Force Release if build is not specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
option(REUSS_USE_SANITIZER "Sanitizers for debugging" OFF)
option(REUSS_TUNE_LOCAL "Tune to the specific CPU" OFF)
option(REUSS_BUILD_TESTS "Build tests" OFF)
option(REUSS_BUILD_PYTHON "Build Python Interface" ON)
option(REUSS_EXTERNAL_SLSDET "use findpackage for slsDetectorPackage instead of building it" OFF)

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
set(CMAKE_INSTALL_RPATH $ORIGIN)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

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
                            -Wconversion
                            -Wnull-dereference
                            -Wdouble-promotion
                            -Werror=return-type
                            -Wpedantic
                            -Wcast-align
)
add_library(reuss::settings ALIAS project_settings)

if(REUSS_TUNE_LOCAL)
    target_compile_options(project_settings INTERFACE -mtune=native -march=native)
endif()

find_package(ClangFormat)
find_package(fmt REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(REUSS_BUILD_TESTS)
  find_package(Catch2 REQUIRED)
  add_subdirectory(tests)
endif(REUSS_BUILD_TESTS)

add_subdirectory(core)
add_subdirectory(math)



if(REUSS_EXTERNAL_SLSDET)
    find_package(slsDetectorPackage REQUIRED)
    #if we are not building slsDetectorPackage, we need to 
    #find zmq as well
    find_package(ZeroMQ 4 REQUIRED) 
else()
    #Build only detector static libs, nothing else is needed for control
    set(SLS_USE_RECEIVER false CACHE BOOL "Disable Receiver")
    set(SLS_USE_RECEIVER_BINARIES false CACHE BOOL "Disable Receiver")
    set(SLS_USE_TEXTCLIENT false CACHE BOOL "Disable Receiver")
    set(SLS_BUILD_SHARED_LIBRARIES false CACHE BOOL "Disable Receiver")
    add_subdirectory(libs/slsDetectorPackage)
endif()

add_subdirectory(detector)
add_subdirectory(components)
add_subdirectory(app)


if(REUSS_BUILD_PYTHON)
  FetchContent_Declare(
    pybind11
    URL ${CMAKE_SOURCE_DIR}/libs/pybind11/v2.11.0.tar.gz
    URL_HASH MD5=90c4946e87c64d8d8fc0ae4edf35d780
  )
  FetchContent_MakeAvailable(pybind11)

  #Take python from conda, TODO! deal with non virtualenv
  set(Python_FIND_VIRTUALENV "ONLY" CACHE STRING "Prefer conda")
  find_package (Python 3.9 COMPONENTS Interpreter Development)
  add_subdirectory(python)
endif()


