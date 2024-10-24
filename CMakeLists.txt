cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 20)
project(platformer_prototype)

# Find SDL2 package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include cxxopts headers
include_directories(external/cxxopts/include)

# Include Box2D headers
include_directories(external/box2d/include)

# Add Box2D directory
add_subdirectory(external/box2d)

# Define source files
set(SOURCE_FILES src/main.cpp)

# Add executable target
add_executable(platformer_prototype ${SOURCE_FILES})

# Link SDL2 and Box2D libraries
target_link_libraries(platformer_prototype box2d ${SDL2_LIBRARIES})

# Include build type options for debug and release builds
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Add compiler flags for debug and release builds
if (WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Add ENABLE_CLANG_TIDY option
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

# Add clang-tidy to CMake configuration when ENABLE_CLANG_TIDY is enabled
if(ENABLE_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

# Add a custom target for running clang-tidy
if(ENABLE_CLANG_TIDY)
  add_custom_target(clang-tidy
    COMMAND ${CMAKE_COMMAND} -E echo "Running clang-tidy"
    COMMAND clang-tidy ${SOURCE_FILES} -- -I${SDL2_INCLUDE_DIRS} -Iexternal/cxxopts/include -Iexternal/box2d/include
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy"
  )
endif()
