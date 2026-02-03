cmake_minimum_required(VERSION 3.20)

# This CMakeLists.txt is included from the module root when:
#   -DVIX_CONVERSION_BUILD_TESTS=ON

include(CTest)
enable_testing()

# ------------------------------------------------------------
# Policy:
# - Prefer GoogleTest if the umbrella provides it.
# - Otherwise, build plain executable tests that return 0/1.
# ------------------------------------------------------------

set(VIX_CONVERSION_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE VIX_CONVERSION_TEST_SOURCES
  "${VIX_CONVERSION_TESTS_DIR}/*.cpp"
)

if (NOT VIX_CONVERSION_TEST_SOURCES)
  message(STATUS "[conversion][tests] No test sources found.")
  return()
endif()

# Helper function to register a test executable
function(vix_conversion_add_plain_test name src)
  add_executable(${name} ${src})
  target_compile_features(${name} PRIVATE cxx_std_20)
  target_link_libraries(${name} PRIVATE vix::conversion)

  if (TARGET vix_warnings)
    target_link_libraries(${name} PRIVATE vix_warnings)
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(${name} PRIVATE vix_sanitizers)
  endif()

  add_test(NAME ${name} COMMAND ${name})
endfunction()

# ------------------------------------------------------------
# GoogleTest path (preferred)
# ------------------------------------------------------------
if (TARGET GTest::gtest AND TARGET GTest::gtest_main)
  message(STATUS "[conversion][tests] Using GoogleTest.")

  add_executable(vix_conversion_tests ${VIX_CONVERSION_TEST_SOURCES})
  target_compile_features(vix_conversion_tests PRIVATE cxx_std_20)
  target_link_libraries(vix_conversion_tests
    PRIVATE
      vix::conversion
      GTest::gtest
      GTest::gtest_main
  )

  if (TARGET vix_warnings)
    target_link_libraries(vix_conversion_tests PRIVATE vix_warnings)
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_conversion_tests PRIVATE vix_sanitizers)
  endif()

  include(GoogleTest)
  gtest_discover_tests(vix_conversion_tests)

else()
  message(STATUS "[conversion][tests] GoogleTest not found. Building plain tests.")

  # Plain tests: one executable per file (simple and robust)
  foreach(src ${VIX_CONVERSION_TEST_SOURCES})
    get_filename_component(fname ${src} NAME_WE)
    set(tname "vix_conversion_test_${fname}")
    vix_conversion_add_plain_test(${tname} ${src})
  endforeach()
endif()
