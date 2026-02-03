#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira. All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp - Conversion Module
# ====================================================================
# Purpose:
#   Low-level, explicit type conversion helpers for Vix.
#   Focus: string_view -> int/bool/float/enum, formatting helpers,
#   and error-friendly APIs using expected-like results.
#   Builds as STATIC when sources exist, otherwise as a header-only
#   INTERFACE target.
#
# Public Targets:
#   - vix_conversion  : The actual library target (STATIC or INTERFACE)
#   - vix::conversion : Namespaced alias for consumers
#
# Public Dependencies:
#   - (none by default)
#
# Installation/Export:
#   Installs into the umbrella export-set `VixTargets`.
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(vix_conversion VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Umbrella build policy:
# When building inside the Vix umbrella, dependencies are managed by the
# umbrella (add_subdirectory order + options). Conversion must not
# FetchContent nor add sibling subdirectories by default.
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_CONVERSION_FETCH_DEPS OFF CACHE BOOL "Auto-fetch deps if missing" FORCE)
endif()

# Sources discovery
file(GLOB_RECURSE CONVERSION_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Optional dependency example (OFF by default)
# Keep conversion minimal and standalone.
option(VIX_CONVERSION_USE_UTILS "Link vix::utils if available (optional)" OFF)
set(VIX_UTILS_TARGET "")

if (VIX_CONVERSION_USE_UTILS)
  if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
    if (NOT TARGET vix::utils)
      message(FATAL_ERROR "[conversion] Umbrella build: VIX_CONVERSION_USE_UTILS=ON but vix::utils target is missing.")
    endif()
    set(VIX_UTILS_TARGET vix::utils)
  else()
    if (TARGET vix::utils)
      set(VIX_UTILS_TARGET vix::utils)
    elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../utils/CMakeLists.txt")
      message(STATUS "[conversion] Adding utils from umbrella: ../utils")
      add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../utils" "utils")
      set(VIX_UTILS_TARGET vix::utils)
    else()
      message(FATAL_ERROR "[conversion] VIX_CONVERSION_USE_UTILS=ON but vix::utils not found. Provide it or turn this option OFF.")
    endif()
  endif()
endif()

# STATIC
if (CONVERSION_SOURCES)
  message(STATUS "[conversion] Building STATIC library with detected sources.")

  add_library(vix_conversion STATIC ${CONVERSION_SOURCES})
  add_library(vix::conversion ALIAS vix_conversion)

  target_compile_features(vix_conversion PUBLIC cxx_std_20)

  target_include_directories(vix_conversion
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  if (VIX_UTILS_TARGET)
    target_link_libraries(vix_conversion PUBLIC ${VIX_UTILS_TARGET})
  endif()

  if (TARGET vix_warnings)
    target_link_libraries(vix_conversion PUBLIC vix_warnings)
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_conversion PUBLIC vix_sanitizers)
  endif()

  set_target_properties(vix_conversion PROPERTIES
    OUTPUT_NAME vix_conversion
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME conversion
  )

  install(TARGETS vix_conversion
    EXPORT VixTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
  endif()

# HEADER-ONLY
else()
  message(STATUS "[conversion] Building HEADER-ONLY library (no sources).")

  add_library(vix_conversion INTERFACE)
  add_library(vix::conversion ALIAS vix_conversion)

  target_compile_features(vix_conversion INTERFACE cxx_std_20)

  target_include_directories(vix_conversion INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  if (VIX_UTILS_TARGET)
    target_link_libraries(vix_conversion INTERFACE ${VIX_UTILS_TARGET})
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_conversion INTERFACE vix_sanitizers)
  endif()

  install(TARGETS vix_conversion
    EXPORT VixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
  endif()

endif()

# Tests
option(VIX_CONVERSION_BUILD_TESTS "Build conversion module tests" OFF)

if (VIX_CONVERSION_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# Summary
message(STATUS "------------------------------------------------------")
message(STATUS "vix::conversion configured (${PROJECT_VERSION})")
if (CONVERSION_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
if (VIX_UTILS_TARGET)
  message(STATUS "Utils target: ${VIX_UTILS_TARGET}")
else()
  message(STATUS "Utils target: (none)")
endif()
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/conversion/...>)")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
