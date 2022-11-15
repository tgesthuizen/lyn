cmake_minimum_required(VERSION 3.0.2)

project(spp CXX)

add_executable(compiler
  src/parser.cpp
  src/lexer.cpp
  src/main.cpp
)
target_include_directories(compiler PUBLIC include)
