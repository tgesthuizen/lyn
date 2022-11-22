cmake_minimum_required(VERSION 3.10)

project(liblyn ASM)

add_library(lyn STATIC
  add.s
  and.s
  const.s
  div.s
  eq.s
  ge.s
  gt.s
  land.s
  le.s
  lor.s
  lt.s
  lxor.s
  mod.s
  mul.s
  neg.s
  neq.s
  or.s
  shl.s
  shr.s
  sub.s
  xor.s
)

install(TARGETS lyn)
