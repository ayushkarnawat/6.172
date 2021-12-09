project(p1)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PRODUCT everybit)

# Debug/Release flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG") # disables assert() checks

# Sources
add_executable(${PRODUCT}
  ${PROJECT_SOURCE_DIR}/src/bitarray.c
  ${PROJECT_SOURCE_DIR}/src/ktiming.c
  ${PROJECT_SOURCE_DIR}/src/main.c
  ${PROJECT_SOURCE_DIR}/src/tests.c
)

# Public headers
target_include_directories(${PRODUCT} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

# Linking
set(CLANG_LDFLAGS "-arch x86_64 -framework CoreServices")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CLANG_LDFLAGS}")
set_target_properties(${PRODUCT} PROPERTIES LINKER_LANGUAGE C)

# Copy data files each time we are building
file(GLOB test_files RELATIVE ${PROJECT_SOURCE_DIR}/data
  ${PROJECT_SOURCE_DIR}/data/*)
foreach(file ${test_files})
  add_custom_command(
    TARGET ${PRODUCT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/data/${file}
            ${CMAKE_CURRENT_BINARY_DIR}/data/${file}
  )
endforeach()
