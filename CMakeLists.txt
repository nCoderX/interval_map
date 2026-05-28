cmake_minimum_required(VERSION 3.20)
project(interval_map VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer vcpkg if VCPKG_ROOT or manifest mode is active
if(DEFINED ENV{VCPKG_ROOT} OR EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/$ENV{VCPKG_DEFAULT_TRIPLET}")
endif()

add_library(interval_map INTERFACE)
target_include_directories(interval_map INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# --- Tests ---
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    find_package(Catch2 3 REQUIRED)

    add_executable(interval_map_tests
        tests/test_interval_map.cpp
        tests/test_overhead.cpp
    )
    target_link_libraries(interval_map_tests PRIVATE
        interval_map
        Catch2::Catch2WithMain
    )
    target_include_directories(interval_map_tests PRIVATE include)

    include(CTest)
    include(Catch)
    catch_discover_tests(interval_map_tests)
endif()
