cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG main
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
  compiler-tests
  symbol_table_tests.cpp
)
target_link_libraries(compiler-tests
  PUBLIC
  compiler
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(compiler-tests)
