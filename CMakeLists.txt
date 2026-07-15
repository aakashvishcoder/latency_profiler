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

# Stress test executable
add_executable(stress_test tests/stress_test.cpp)
target_link_libraries(stress_test PRIVATE latency_profiler::latency_profiler)

add_custom_target(run_stress_test
    COMMAND ${CMAKE_COMMAND} -E env
        ${CMAKE_CURRENT_BINARY_DIR}/stress_test${CMAKE_EXECUTABLE_SUFFIX}
    DEPENDS stress_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
    COMMENT "Build and run stress_test"
)

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

option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
if(BUILD_PYTHON_BINDINGS)
    set(PYBIND11_FINDPYTHON ON CACHE BOOL "Use modern FindPython in pybind11" FORCE)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    find_package(pybind11 CONFIG REQUIRED)

    pybind11_add_module(latency_profiler_py bindings/profiler_bindings.cpp)
    target_link_libraries(latency_profiler_py PRIVATE latency_profiler::latency_profiler)

    if(MINGW)
        # Make the extension self-contained so it can be imported without MSYS2 runtime PATH tweaks.
        target_link_options(latency_profiler_py PRIVATE -static-libgcc -static-libstdc++)

        get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        add_custom_command(TARGET latency_profiler_py POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MINGW_BIN_DIR}/libwinpthread-1.dll
                $<TARGET_FILE_DIR:latency_profiler_py>
        )
    endif()

    set_target_properties(latency_profiler_py PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_custom_target(run_dashboard
        COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}
            ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dashboard.py
        DEPENDS latency_profiler_py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        USES_TERMINAL
        COMMENT "Build latency_profiler_py and run dashboard.py"
    )
endif()