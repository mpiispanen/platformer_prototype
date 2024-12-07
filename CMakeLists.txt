cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(platformer_prototype)

# Conditionally enable Objective-C language support on macOS
if (APPLE)
    enable_language(OBJC)
    
    # Define source files
    file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)
endif()

# Define source files
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)


# Add executable target
add_executable(platformer_prototype ${SOURCE_FILES})

# Include and link external libraries

## Box2D
add_subdirectory(external/box2d)
target_include_directories(platformer_prototype PRIVATE external/box2d/include)

## cxxopts
target_include_directories(platformer_prototype PRIVATE external/cxxopts/include)

## nlohmann/json
add_subdirectory(external/nlohmann)
target_include_directories(platformer_prototype PRIVATE external/json/include)

## SDL3
add_subdirectory(external/sdl)
target_include_directories(platformer_prototype PRIVATE external/sdl/include)

## SDL2_image
add_subdirectory(external/sdl_image)
target_include_directories(platformer_prototype PRIVATE external/sdl_image/include)

## spdlog
add_subdirectory(external/spdlog)
target_include_directories(platformer_prototype PRIVATE external/spdlog/include)

## Dear ImGui
# Add the include directories for imgui and its backends
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

# Add the imgui source files
file(GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

# Ensure SDL3 include directories are specified for imgui backends
target_include_directories(imgui PRIVATE external/sdl/include)

# Link the imgui library to the executable
target_link_libraries(platformer_prototype PRIVATE imgui)

# Link SDL3, Box2D, and nlohmann_json libraries
target_link_libraries(platformer_prototype PRIVATE SDL3::SDL3 SDL3_image::SDL3_image box2d nlohmann_json::nlohmann_json spdlog::spdlog imgui)

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

  # Create a custom target for running Clang-Tidy
  add_custom_target(run-clang-tidy
    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${SOURCE_FILES}
      --checks=${CLANG_TIDY_CHECKS}
      --format-style=file
      --export-fixes=${CMAKE_BINARY_DIR}/clang-tidy-fixes.yaml
      --
      -x c++
      -std=c++20
      -I${CMAKE_SOURCE_DIR}/src
      -I${CMAKE_SOURCE_DIR}/external/cxxopts/include
      -I${CMAKE_SOURCE_DIR}/external/box2d/include
      -I${CMAKE_SOURCE_DIR}/external/nlohmann/include
      -I${CMAKE_SOURCE_DIR}/external/sdl/include
      -I${CMAKE_SOURCE_DIR}/external/sdl_image/include
      -I${CMAKE_SOURCE_DIR}/external/spdlog/include
      -I${CMAKE_SOURCE_DIR}/external/imgui
      -I${CMAKE_SOURCE_DIR}/external/imgui/backends
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy on source files"
  )

  # Create a custom target for applying Clang-Tidy fixes
  add_custom_target(apply-clang-tidy-fixes
    COMMAND ${CMAKE_COMMAND} -E echo "Applying Clang-Tidy fixes"
    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${SOURCE_FILES}
      -fix
      -fix-errors
      --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
      --
      -x c++
      -std=c++20
      -I${CMAKE_SOURCE_DIR}/src
      -I${CMAKE_SOURCE_DIR}/external/cxxopts/include
      -I${CMAKE_SOURCE_DIR}/external/box2d/include
      -I${CMAKE_SOURCE_DIR}/external/nlohmann/include
      -I${CMAKE_SOURCE_DIR}/external/sdl/include
      -I${CMAKE_SOURCE_DIR}/external/sdl_image/include
      -I${CMAKE_SOURCE_DIR}/external/spdlog/include
      -I${CMAKE_SOURCE_DIR}/external/imgui
      -I${CMAKE_SOURCE_DIR}/external/imgui/backends
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Applying Clang-Tidy fixes"
  )
endif()
