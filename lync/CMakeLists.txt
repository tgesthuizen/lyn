cmake_minimum_required(VERSION 3.10)

project(lyn CXX)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        12.1.0
)

FetchContent_MakeAvailable(fmt)

option(LYNC_ENABLE_TESTS "Whether to build tests for the lyn compiler" ON)
if(${LYNC_ENABLE_TESTS})
  enable_testing()
endif()

add_library(compiler STATIC
  src/alpha_convert.cpp
  src/anf.cpp
  src/genasm.cpp
  src/parser.cpp
  src/primitives.cpp
  src/print-anf.cpp
  src/typecheck.cpp
)
target_include_directories(compiler PUBLIC include)
target_include_directories(compiler PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/parser
)
target_compile_features(compiler PUBLIC cxx_std_23)
target_link_libraries(compiler PUBLIC fmt::fmt)

add_executable(lync
  main.cpp
)
target_link_libraries(lync PUBLIC compiler)

install(TARGETS lync)

if(${LYNC_ENABLE_TESTS})
  add_subdirectory(tests)
endif()
