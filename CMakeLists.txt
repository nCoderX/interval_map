cmake_minimum_required(VERSION 3.20)
project(interval_map VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(interval_map INTERFACE)
target_include_directories(interval_map INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# --- Tests ---
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    # Try vcpkg first, then fall back to FetchContent.
    find_package(Catch2 3 QUIET)

    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.6.0
        )
        FetchContent_MakeAvailable(Catch2)
    endif()

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

# --- Examples ---
option(BUILD_EXAMPLES "Build usage examples" ON)
if(BUILD_EXAMPLES)
    add_executable(example_resource_booking examples/resource_booking.cpp)
    target_link_libraries(example_resource_booking PRIVATE interval_map)
    target_include_directories(example_resource_booking PRIVATE include)

    add_executable(example_price_tiers examples/price_tiers.cpp)
    target_link_libraries(example_price_tiers PRIVATE interval_map)
    target_include_directories(example_price_tiers PRIVATE include)

    add_executable(example_time_config examples/time_based_config.cpp)
    target_link_libraries(example_time_config PRIVATE interval_map)
    target_include_directories(example_time_config PRIVATE include)
endif()
