set(DEVICES_WT901C_SRCS
    tilt_wt901c.cpp
)

unified_component_register(
  TARGET devices
  SRCS ${DEVICES_WT901C_SRCS}
  INCLUDE_DIRS include
  PUBLIC_LIBS core hal        # Host: linke gegen core/hal
  # REQUIRES <idf comps>      # ESP-IDF: falls später benötigt
  CXX_STANDARD 17
)