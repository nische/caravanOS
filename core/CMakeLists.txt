set(CORE_SRCS
  metric.cpp
  metric_bus.cpp
  include/core/device_base.hpp
  include/core/enum_hash.hpp
)

unified_component_register(
  TARGET core
  SRCS ${CORE_SRCS}
  INCLUDE_DIRS include
  CXX_STANDARD 17
)
