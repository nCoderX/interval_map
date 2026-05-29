# interval_map

A modern, efficient C++ container for mapping half-open intervals [keyBegin, keyEnd) to values, with automatic merging of adjacent intervals that share the same value.

This implementation is inspired by the ThinkCell C++ recruitment test but modernized for practical use.

## Building

```bash
cmake --preset default
cmake --build --preset Build
```

## Running Unit Tests

```bash
ctest --preset Test --output-on-failure
```

## Basic Usage

```cpp
#include <interval_map/interval_map.hpp>

interval_map::IntervalMap<int, std::string> im("default");
im.assign(10, 20, "special");
im.assign(15, 30, "other");
```

## License

MIT
