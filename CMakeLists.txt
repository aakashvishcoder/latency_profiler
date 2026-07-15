cmake_minimum_required(VERSION 3.16)
project(LatencyProfiler 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "Low-latency, zero-allocation C++ profiler"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the INTERFACE library (header-only style, but with a target)
add_library(latency_profiler INTERFACE)
add_library(latency_profiler::latency_profiler ALIAS latency_profiler)

# Include directories
target_include_directories(latency_profiler INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Require C++17 for consumers
target_compile_features(latency_profiler INTERFACE cxx_std_17)

# Installation rules
include(GNUInstallDirs)
install(TARGETS latency_profiler
    EXPORT latency_profiler-targets
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT latency_profiler-targets
    FILE latency_profiler-config.cmake
    NAMESPACE latency_profiler::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/latency_profiler
)

include(FetchContent)

FetchContent_Declare(
    latency_profiler
    GIT_REPOSITORY https://github.com/aakashvishcoder/latency_profiler.git
    GIT_TAG        v1.0.0
)

FetchContent_MakeAvailable(latency_profiler)

# Link it to your trading engine
target_link_libraries(your_trading_engine PRIVATE latency_profiler::latency_profiler)

find_package(pybind11 REQUIRED)

pybind11_add_module(latency_profiler_py profiler_bindings.cpp)
target_link_libraries(latency_profiler_py PRIVATE latency_profiler::latency_profiler)