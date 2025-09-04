# cmake/UnifiedComponent.cmake
# Unified registration for modules that should work in both
# pure CMake (host) and ESP-IDF (component) contexts.
#
# Usage:
#   unified_component_register(
#     TARGET <name>
#     [INTERFACE_ONLY]
#     SRCS <files...>
#     INCLUDE_DIRS <dirs...>
#     PUBLIC_LIBS <targets...>      # host-only
#     PRIVATE_LIBS <targets...>     # host-only
#     REQUIRES <idf components...>  # idf-only
#     PRIV_REQUIRES <idf components...> # idf-only
#     CXX_STANDARD <17|20|...>
#     DEFINES <FOO=1 BAR ...>
#   )
#
include(CMakeParseArguments)

function(unified_component_register)
  set(options INTERFACE_ONLY)
  set(oneValueArgs TARGET CXX_STANDARD)
  set(multiValueArgs SRCS INCLUDE_DIRS REQUIRES PRIV_REQUIRES PUBLIC_LIBS PRIVATE_LIBS DEFINES)
  cmake_parse_arguments(UCR "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT UCR_TARGET)
    message(FATAL_ERROR "unified_component_register: TARGET is required")
  endif()

  # Are we inside an ESP-IDF build?
  if (COMMAND idf_component_register)
    # IDF branch
    if (UCR_INTERFACE_ONLY)
      idf_component_register(
        SRCS          ${UCR_SRCS}
        INCLUDE_DIRS  ${UCR_INCLUDE_DIRS}
        REQUIRES      ${UCR_REQUIRES}
        PRIV_REQUIRES ${UCR_PRIV_REQUIRES}
      )
    else()
      idf_component_register(
        SRCS          ${UCR_SRCS}
        INCLUDE_DIRS  ${UCR_INCLUDE_DIRS}
        REQUIRES      ${UCR_REQUIRES}
        PRIV_REQUIRES ${UCR_PRIV_REQUIRES}
      )
    endif()
    if (UCR_CXX_STANDARD)
      set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD ${UCR_CXX_STANDARD})
    else()
      set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
    endif()
    if (UCR_DEFINES)
      target_compile_definitions(${COMPONENT_LIB} PUBLIC ${UCR_DEFINES})
    endif()
  else()
    # Host CMake branch
    if (UCR_INTERFACE_ONLY)
      add_library(${UCR_TARGET} INTERFACE)
      if (UCR_INCLUDE_DIRS)
        target_include_directories(${UCR_TARGET} INTERFACE ${UCR_INCLUDE_DIRS})
      endif()
      if (UCR_PUBLIC_LIBS)
        target_link_libraries(${UCR_TARGET} INTERFACE ${UCR_PUBLIC_LIBS})
      endif()
      if (UCR_DEFINES)
        target_compile_definitions(${UCR_TARGET} INTERFACE ${UCR_DEFINES})
      endif()
    else()
      add_library(${UCR_TARGET} STATIC ${UCR_SRCS})
      if (UCR_INCLUDE_DIRS)
        target_include_directories(${UCR_TARGET} PUBLIC ${UCR_INCLUDE_DIRS})
      endif()
      if (UCR_PUBLIC_LIBS)
        target_link_libraries(${UCR_TARGET} PUBLIC ${UCR_PUBLIC_LIBS})
      endif()
      if (UCR_PRIVATE_LIBS)
        target_link_libraries(${UCR_TARGET} PRIVATE ${UCR_PRIVATE_LIBS})
      endif()
      if (UCR_CXX_STANDARD)
        target_compile_features(${UCR_TARGET} PUBLIC cxx_std_${UCR_CXX_STANDARD})
      else()
        target_compile_features(${UCR_TARGET} PUBLIC cxx_std_17)
      endif()
      if (UCR_DEFINES)
        target_compile_definitions(${UCR_TARGET} PUBLIC ${UCR_DEFINES})
      endif()
    endif()
  endif()
endfunction()
