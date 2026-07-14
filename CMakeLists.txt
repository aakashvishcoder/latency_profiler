cmake_minimum_required(VERSION 3.16)
project(LatencyProfiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aggressive optimization for benchmark builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -DNDEBUG)
endif()

add_executable(stress_test tests/stress_test.cpp)
target_include_directories(stress_test PRIVATE include)
target_link_libraries(stress_test PRIVATE pthread)