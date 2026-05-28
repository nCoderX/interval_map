# interval_map

A modern, efficient C++ container for mapping **half-open intervals** `[keyBegin, keyEnd)` to values, with automatic merging of adjacent intervals that share the same value.

This implementation is inspired by the famous **ThinkCell C++ recruitment test** but has been modernized for real-world use.

## Why Use an Interval Map?

An `IntervalMap` is extremely useful whenever you need to associate **ranges** of values with data, rather than individual points.

### Real-World Use Cases

| Domain                    | Example Use Case                                      | Value Type     |
|---------------------------|-------------------------------------------------------|----------------|
| **Resource Scheduling**   | Book meeting rooms, machines, or people over time     | `std::string` (person name) |
| **Pricing / Billing**     | Tiered pricing based on quantity or time              | `double` (price per unit) |
| **Configuration**         | Feature flags or logging levels that change over time | `std::string` |
| **Memory Management**     | Track allocated / freed memory regions                | Allocation metadata |
| **Version Control**       | Annotate ranges of a file with blame or review status | Reviewer name |
| **Network / IP**          | Map IP address ranges to organizations or policies    | Policy object |
| **Calendar / Events**     | Store events with start/end times                     | Event details |

## Basic Usage

```cpp
#include <interval_map/interval_map.hpp>
#include <iostream>
#include <string>

int main() {
    using namespace interval_map;

    IntervalMap<int, std::string> schedule("Free");

    schedule.assign(9, 12, "Alice");
    schedule.assign(10, 14, "Bob");        // Overwrites part of Alice's booking
    schedule.assign(14, 16, "Charlie");

    for (int hour = 8; hour < 18; ++hour) {
        std::cout << hour << ":00 → " << schedule[hour] << '\n';
    }
}
```

Output:
```
8:00 → Free
9:00 → Alice
10:00 → Bob
...
13:00 → Bob
14:00 → Charlie
```

## Building

### Recommended: Using vcpkg (Manifest Mode)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Explicit vcpkg Toolchain

```bash
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Running Tests and Benchmarks

```bash
./build/interval_map_tests
```

This runs both unit tests and performance benchmarks.

## Performance

The implementation is designed to be efficient for the common case of range updates:

```
-------------------------------------------------------------------------------
10000 sequential small assigns
-------------------------------------------------------------------------------
5000 random assigns on [0,10000]
-------------------------------------------------------------------------------
1000 heavily overlapping assigns
```

See the benchmark output in `tests/test_interval_map.cpp` for current numbers on your machine.

## Design Notes

- The container is **header-only**.
- The default value (`valBegin`) is **never** stored inside the internal map.
- Adjacent intervals with the same value are automatically merged.
- `assign()` is the main operation and is optimized to perform a small number of comparisons and value copies.

## Limitations / Current Status

- The `assign()` implementation is good but not yet perfect on every possible edge case (some advanced merge scenarios are still being hardened).
- No custom iterator that yields full intervals (only the underlying change points are exposed via `intervals()`).

## Contributing

Contributions, especially improvements to the `assign()` algorithm and additional test cases, are very welcome.

## License

MIT License

---

**Originally created as a sample solution for the ThinkCell C++ interview challenge.** This version aims to be a practical, modern library rather than just an interview exercise.