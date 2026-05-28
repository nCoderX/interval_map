cmake_minimum_required(VERSION 3.20)
project(interval_map VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(interval_map INTERFACE)
target_include_directories(interval_map INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Modern tests with Catch2
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(interval_map_tests
        tests/test_interval_map.cpp
        tests/test_overhead.cpp
    )
    target_link_libraries(interval_map_tests PRIVATE interval_map Catch2::Catch2WithMain)
    target_include_directories(interval_map_tests PRIVATE include)

    include(CTest)
    include(Catch)
    catch_discover_tests(interval_map_tests)
endif()
