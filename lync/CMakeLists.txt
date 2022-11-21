cmake_minimum_required(VERSION 3.10)

project(lyn CXX)

find_package(BISON REQUIRED)

BISON_TARGET(
  parser
  src/parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/parser/parser.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser/parser.h
)

add_executable(lync
  ${BISON_parser_OUTPUTS}
  src/anf.cpp
  src/genasm.cpp
  src/lexer.cpp
  src/main.cpp
  src/print-anf.cpp
  src/scopify.cpp
  src/typecheck.cpp
)
target_include_directories(
  lync
  PUBLIC
  include
  ${CMAKE_CURRENT_BINARY_DIR}/parser
)

install(TARGETS lync)
