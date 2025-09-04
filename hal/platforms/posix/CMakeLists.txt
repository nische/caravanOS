# hal/platforms/posix/CMakeLists.txt
include(${CMAKE_SOURCE_DIR}/cmake/UnifiedComponent.cmake)

unified_component_register(
  TARGET hal_posix
  SRCS
    clock_posix.cpp
    modbus_dummy.cpp
  INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
  PUBLIC_LIBS
    hal_interface
  CXX_STANDARD 17
)
