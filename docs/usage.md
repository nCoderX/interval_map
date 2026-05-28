# Usage Guide

## Construction

```cpp
// Default value is required
interval_map::IntervalMap<int, std::string> im("default_value");
```

## Assigning Values to Ranges

```cpp
im.assign(10, 20, "special");           // [10, 20) → "special"
im.assign(15, 30, "other");             // partial overwrite
im.assign(100, "from_here_onward");     // from 100 to "infinity"
```

## Looking Up Values

```cpp
std::string value = im[42];             // O(log n)
```

## Inspecting Internal State

```cpp
const auto& changes = im.intervals();   // std::map<K, V> of change points
for (const auto& [start, value] : changes) {
    std::cout << "From " << start << " → " << value << '\n';
}
```

## Other Operations

- `clear()`
- `swap()`
- Range-based for loop over `begin()` / `end()` (yields change points)

## Requirements on Key and Value Types

- **Key (`K`)**: Must support `operator<`, be copyable and movable.
- **Value (`V`)**: Must support `operator==`, be copyable and movable.
