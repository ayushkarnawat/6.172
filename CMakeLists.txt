cmake_minimum_required(VERSION 3.19)

project(6.172 LANGUAGES C)

# specify (default) C standard; projects can override as needed
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  message( STATUS "Debug mode enabled" )
endif()

# WARNING: the order of the following is normally important when building
#          multiple modules; however here since eacn project is independent of
#          each other, the order does not matter (for now)
add_subdirectory(projects)
