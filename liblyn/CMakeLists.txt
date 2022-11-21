cmake_minimum_required(VERSION 3.10)

project(liblyn ASM)

add_library(lyn STATIC
  add.s
  eq.s
  mul.s
  sub.s
)
set_source_files_properties(eq.S PROPERTIES LANGUAGE ASM)

install(TARGETS lyn)
