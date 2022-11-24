cmake_minimum_required(VERSION 3.10)

project(lyn CXX)

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

add_executable(lync
  main.cpp
)
target_link_libraries(lync PUBLIC compiler)

install(TARGETS lync)
