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
option(REUSS_BUILD_GUI "Gui for live data visualization" ON)
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
)
if(REUSS_TUNE_LOCAL)
    target_compile_options(project_settings INTERFACE -mtune=native -march=native)
endif()


find_package(ClangFormat)
find_package(fmt REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# find_package(ZeroMQ 4 REQUIRED) #use the cppzmq workaround?

add_subdirectory(core)
add_subdirectory(app)


set(SLS_USE_RECEIVER false CACHE BOOL "Disable Receiver")
add_subdirectory(libs/slsDetectorPackage)
add_subdirectory(interface)

find_package (Python 3.6 COMPONENTS Interpreter Development)
add_subdirectory(libs/pybind11)
add_subdirectory(python)


if(REUSS_BUILD_GUI)
  set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/libs/imgui" CACHE PATH "Imgui top level dir")
  set(IMGUI_SFML_FIND_SFML false CACHE BOOL "Look for sfml?")
  add_subdirectory(libs/SFML)
  add_subdirectory(libs/imgui-sfml)
  add_subdirectory(libs/glfw)
  add_subdirectory(gui)
endif()

