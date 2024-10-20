cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 20)
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

# Include cxxopts headers
include_directories(external/cxxopts/include)

# Add ENABLE_COVERAGE option
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Add ENABLE_CLANG_TIDY option
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

# Add coverage flags to compiler options when ENABLE_COVERAGE is enabled
if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Add clang-tidy to CMake configuration when ENABLE_CLANG_TIDY is enabled
if(ENABLE_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

# Add a custom target for running clang-tidy
if(ENABLE_CLANG_TIDY)
  add_custom_target(clang-tidy
    COMMAND ${CMAKE_COMMAND} -E echo "Running clang-tidy"
    COMMAND clang-tidy ${SOURCE_FILES} -I${SDL2_INCLUDE_DIRS} --
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy"
  )
endif()
