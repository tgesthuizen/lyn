cmake_minimum_required(VERSION 3.0.2)

project(spp CXX)

find_package(BISON REQUIRED)

BISON_TARGET(
  parser
  src/parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/parser/parser.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser/parser.h
)

add_executable(compiler
  ${BISON_parser_OUTPUTS}
  src/lexer.cpp
  src/main.cpp
)
target_include_directories(compiler PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/parser)
