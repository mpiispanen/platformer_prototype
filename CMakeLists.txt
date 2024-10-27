cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(platformer_prototype)

# Define source files
set(SOURCE_FILES src/main.cpp)

# Add executable target
add_executable(platformer_prototype ${SOURCE_FILES})

# Include and link external libraries

## Box2D
add_subdirectory(external/box2d)
target_include_directories(platformer_prototype PRIVATE external/box2d/include)

## cxxopts
target_include_directories(platformer_prototype PRIVATE external/cxxopts/include)

## SDL3
add_subdirectory(external/sdl)
target_include_directories(platformer_prototype PRIVATE external/sdl/include)

# Link SDL3 and Box2D libraries
target_link_libraries(platformer_prototype PRIVATE SDL3::SDL3 box2d)

# Include build type options for debug and release builds
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Add compiler flags for debug and release builds
if (WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Add ENABLE_CLANG_TIDY option
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

# Enable Clang-Tidy if the option is ON
if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE clang-tidy REQUIRED)

  # Define the Clang-Tidy checks you want to perform
  set(CLANG_TIDY_CHECKS "-*,modernize-*,readability-*")  # Customize as needed

  # Create a custom target for Clang-Tidy
  add_custom_target(run-clang-tidy
    COMMAND ${CLANG_TIDY_EXE} ${SOURCE_FILES}
      --checks=${CLANG_TIDY_CHECKS}
      --warnings-as-errors=*  # Treat all warnings as errors
      --format-style=file      # Use .clang-format file for formatting
      --export-fixes=clang-tidy-fixes.yaml  # Export fixes to a YAML file
      -- -Iexternal/cxxopts/include -Iexternal/box2d/include -Iexternal/sdl/include
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy on source files with report generation"
  )
endif()
