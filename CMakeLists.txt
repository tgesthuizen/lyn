cmake_minimum_required(VERSION 3.10)

project(spp CXX)

include(ExternalProject)

set(CMAKE_TARGET_ARGS
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/arm-none-eabi.cmake"
    CACHE LIST
    "CMake arguments for target architecture tools")

add_subdirectory(lync)
ExternalProject_Add(liblyn
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblyn
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/liblyn
  CMAKE_ARGS ${CMAKE_TARGET_ARGS}
  INSTALL_COMMAND ""
  ALWAYS YES
)
