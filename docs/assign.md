# The assign() Operation

This is the heart of `IntervalMap`.

## Semantics

`assign(keyBegin, keyEnd, value)` has the following effects:

- Every key `k` where `keyBegin <= k < keyEnd` will return `value` when looked up.
- The container maintains the invariant that no two adjacent intervals have the same value.
- The default value is never materialized as an entry in the internal map.

## Performance Characteristics

- Average case: O(log n) comparisons + small number of node allocations.
- The implementation tries to use `std::find_if` + range `erase` to minimize manual iteration.

## Current Status (as of latest commit)

The implementation has good coverage of common and many edge cases. A few very specific boundary conditions around touching ranges and complex overwrite sequences are still being hardened (tracked in the test file with TODO comments).

For most real-world use cases the current version is already production usable.
