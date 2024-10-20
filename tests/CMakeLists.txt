cmake_minimum_required(VERSION 3.10)
project(tests)

add_subdirectory(..)

add_executable(runUnitTests test_main.cpp)
target_link_libraries(runUnitTests GTest::GTest GTest::Main)
