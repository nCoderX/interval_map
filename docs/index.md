# interval_map Documentation

Welcome to the documentation for `interval_map`, a modern C++ header-only container for efficient interval-to-value mapping.

## What is IntervalMap?

`IntervalMap<K, V>` lets you associate **ranges** of keys with values.

Instead of storing one value per key, you store one value per *interval*. Adjacent intervals with the same value are automatically merged.

## Quick Example

```cpp
#include <interval_map/interval_map.hpp>
#include <iostream>

int main() {
    interval_map::IntervalMap<int, std::string> log_level("INFO");

    log_level.assign(100, 500, "DEBUG");   // Experiment window
    log_level.assign(800, 1200, "WARN");   // Maintenance

    std::cout << log_level[150] << '\n';   // DEBUG
    std::cout << log_level[900] << '\n';   // WARN
}
```

## Main Sections

- [Usage Guide](usage.md)
- [The assign() Operation](assign.md) — core semantics and current status
- [Examples](examples.md)
- [Performance](performance.md)

## Building

See the main [README](../README.md) for build instructions using CMake + vcpkg.

## License

MIT License. See [LICENSE](../LICENSE) in the repository root.
