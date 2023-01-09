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
  meta_tests.cpp
  # symbol_table_tests.cpp
)
target_link_libraries(compiler-tests
  PUBLIC
  compiler
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(compiler-tests)

set(LYNC_FAIL_TESTS
  application_return_mismatch.scm
  application_type_mismatch.scm
  clojures_are_not_supported.scm
  if_cond_branches_do_not_unify.scm
  if_cond_not_bool.scm
  non-existent-variable-ref.scm
)
foreach(test ${LYNC_FAIL_TESTS})
  add_test(
    NAME "${test}_fails"
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_fail.sh
            $<TARGET_FILE:lync>
	    ${CMAKE_CURRENT_SOURCE_DIR}/fails/${test}
  )
endforeach()

if(DEFINED LYN_EXAMPLE_DIR)
  foreach(example ${LYN_EXAMPLES})
    add_test(
      NAME "${example}_compiles"
      COMMAND $<TARGET_FILE:lync> ${LYN_EXAMPLE_DIR}/${example}
    )
  endforeach()
endif()
