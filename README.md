# interval_map

Modern C++ implementation of an interval map (range map) container.

This started as a sample solution for the famous **ThinkCell C++ recruitment test**.

## Current Status

- Header-only `IntervalMap<K, V>`
- Modern CMake build (C++23)
- Unit tests using Catch2
- Cross-platform (tested on Linux with GCC 15)

**Note**: The core `assign()` algorithm is still being refined for full correctness and efficiency matching the original challenge requirements. Basic functionality works.

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/interval_map_tests
```

## Usage

```cpp
#include "interval_map/interval_map.hpp"

interval_map::IntervalMap<int, std::string> im("default");

im.assign(10, 20, "special");
im.assign(15, 30, "other");

std::cout << im[12]; // "special"
std::cout << im[25]; // "other"
```

## Roadmap / Improvements

- [ ] Fully correct and efficient `assign()` implementation
- [ ] More comprehensive test coverage (including overhead tests from the original challenge)
- [ ] Iterator support
- [ ] Documentation + examples
- [ ] vcpkg / Conan support

Contributions welcome!

## License

MIT
