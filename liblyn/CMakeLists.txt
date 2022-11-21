cmake_minimum_required(VERSION 3.10)

project(liblyn ASM)

add_library(lyn STATIC
  add.s
  and.s
  eq.s
  ge.s
  gt.s
  land.s
  lor.s
  lt.s
  lxor.s
  mul.s
  neg.s
  neq.s
  shl.s
  shr.s
  sub.s
)

install(TARGETS lyn)
