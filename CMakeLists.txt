cmake_minimum_required(VERSION 3.10)
project(platformer_prototype)

# Find SDL2 package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Define source files
set(SOURCE_FILES src/main.cpp)

# Add executable target
add_executable(platformer_prototype ${SOURCE_FILES})

# Link SDL2 library
target_link_libraries(platformer_prototype ${SDL2_LIBRARIES})

# Include build type options for debug and release builds
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Add compiler flags for debug and release builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
