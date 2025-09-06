cmake_minimum_required(VERSION 3.20)
project(CaravanOS LANGUAGES CXX)

# Global options
option(BUILD_UI "Build UI module" OFF)
option(BUILD_APPS "Build UI module" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add HAL (interface + posix impl → unified 'hal' target)
add_subdirectory(hal)
add_subdirectory(core)
add_subdirectory(devices)

if (BUILD_TESTS)
  enable_testing()
  find_package(GTest REQUIRED)
  add_subdirectory(tests/core)
  add_subdirectory(tests/devices)
endif()
# Hello-world app for Linux
if (BUILD_APPS)
  add_subdirectory(apps/hello_linux)
endif()
