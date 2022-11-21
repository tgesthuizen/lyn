cmake_minimum_required(VERSION 3.10)

project(liblyn ASM)

add_library(lyn STATIC
  add.s
  eq.s
  mul.s
  neq.s
  shr.s
  sub.s
)

install(TARGETS lyn)
