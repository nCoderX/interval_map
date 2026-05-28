# Performance

The library includes built-in benchmarks using Catch2.

Run them with:

```bash
./build/interval_map_tests
```

Typical workloads tested:

- Sequential non-overlapping inserts
- Heavily overlapping range updates
- Random overlapping updates

The current implementation aims for a good balance between simplicity and performance. Further micro-optimizations (reducing comparisons to the theoretical minimum) are possible but increase code complexity significantly.
