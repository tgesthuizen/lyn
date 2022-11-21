cmake_minimum_required(VERSION 3.10)

project(spp CXX)

include(ExternalProject)

add_subdirectory(lync)

set(CMAKE_TARGET_ARGS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/arm-none-eabi.cmake
    CACHE STRING
    "CMake arguments for target architecture tools")

list(APPEND CMAKE_TARGET_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/arm-none-eabi)

ExternalProject_Add(liblyn
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblyn
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/liblyn
  CMAKE_ARGS ${CMAKE_TARGET_ARGS}
  INSTALL_COMMAND ""
  BUILD_ALWAYS YES
)
