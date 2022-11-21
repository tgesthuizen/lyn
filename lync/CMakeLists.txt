cmake_minimum_required(VERSION 3.10)

project(lyn CXX)

find_package(BISON REQUIRED)

BISON_TARGET(
  parser
  src/parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/parser/parser.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser/parser.h
)

add_library(frontend STATIC
  ${BISON_parser_OUTPUTS}
  src/anf.cpp
  src/lexer.cpp
  src/scopify.cpp
  src/typecheck.cpp
)
target_include_directories(frontend PUBLIC include)
target_include_directories(frontend PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/parser
)

add_executable(lync
  asm/genasm.cpp
  asm/main.cpp
)
target_link_libraries(lync PUBLIC frontend)

add_executable(print-anf
  print-anf/print-anf.cpp
  print-anf/main.cpp
)
target_link_libraries(print-anf PUBLIC frontend)

install(TARGETS lync)
