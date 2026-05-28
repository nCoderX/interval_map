# interval_map

Modern C++ implementation of an interval map (range map) container.

This started as a sample solution for the famous **ThinkCell C++ recruitment test**.

## Current Status

- Header-only `IntervalMap<K, V>`
- Modern CMake build (C++23)
- Unit tests + benchmarks using Catch2
- Uses **vcpkg** for dependencies (as preferred)
- Cross-platform

## Building with vcpkg (Recommended)

```bash
# First time setup
vcpkg integrate install          # optional but recommended

cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j
./build/interval_map_tests
```

If you're inside a vcpkg manifest environment (recommended):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Building without vcpkg (not recommended)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
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

- [ ] Fully correct and efficient `assign()` implementation (some edge cases remain)
- [ ] More comprehensive overhead tests matching the original ThinkCell challenge
- [ ] Iterator support
- [x] vcpkg support (done)

Contributions welcome!

## License

MIT
