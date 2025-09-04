# hal/CMakeLists.txt
include(${CMAKE_SOURCE_DIR}/cmake/UnifiedComponent.cmake)

# Always export an interface target with the headers
unified_component_register(
  TARGET hal_interface
  INTERFACE_ONLY
  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Platform selection via HAL_PLATFORM (default: posix)
if(NOT DEFINED HAL_PLATFORM)
  set(HAL_PLATFORM "posix")
endif()
message(STATUS "HAL_PLATFORM = ${HAL_PLATFORM}")

if(HAL_PLATFORM STREQUAL "posix")
  add_subdirectory(platforms/posix)
  add_library(hal INTERFACE)
  target_link_libraries(hal INTERFACE hal_interface hal_posix)
elseif(HAL_PLATFORM STREQUAL "mock")
  add_library(hal INTERFACE)
  target_link_libraries(hal INTERFACE hal_interface)
else()
  message(FATAL_ERROR "Unknown HAL_PLATFORM: ${HAL_PLATFORM} (expected posix|mock)")
endif()
