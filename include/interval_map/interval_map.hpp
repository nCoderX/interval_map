// Copyright (c) 2024
// All rights reserved.
//
// This code is licensed under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <map>
#include <concepts>
#include <type_traits>
#include <utility>

namespace interval_map {

template <typename T>
concept IntervalKey = std::copyable<T> &&
                      std::is_move_constructible_v<T> &&
                      std::is_move_assignable_v<T> &&
                      requires(const T& a, const T& b) {
                          { a < b } -> std::convertible_to<bool>;
                      };

template <typename T>
concept IntervalValue = std::copyable<T> &&
                        std::is_move_constructible_v<T> &&
                        std::is_move_assignable_v<T> &&
                        requires(const T& a, const T& b) {
                            { a == b } -> std::convertible_to<bool>;
                        };

template <typename From, typename To>
concept ForwardableTo = std::is_constructible_v<To, From> &&
                        std::is_assignable_v<To&, From>;

/**
 * @brief Efficient associative container for mapping half-open intervals to values.
 *
 * `IntervalMap<K, V>` represents a mapping from keys to values where large contiguous
 * ranges of keys can share the same value. Only the "change points" are stored
 * internally using a `std::map`.
 *
 * Key properties:
 * - `assign(keyBegin, keyEnd, value)` sets the range `[keyBegin, keyEnd)` to `value`.
 * - Adjacent intervals with identical values are automatically merged.
 * - The initial/default value is **never** stored in the internal map.
 * - Lookup via `operator[]` is O(log n).
 *
 * This data structure is particularly useful for:
 * - Resource scheduling (rooms, machines, personnel over time)
 * - Tiered pricing / billing systems
 * - Time-varying configuration and feature flags
 * - Memory region or IP range tracking
 *
 * The design is inspired by the classic ThinkCell C++ recruitment test, with
 * modern C++23 idioms and a focus on clarity and testability.
 *
 * @tparam K  Key type. Requirements: copyable, movable, and supports `operator<`.
 * @tparam V  Value type. Requirements: copyable, movable, and supports `operator==`.
 *
 * @note Performance: The implementation uses `std::map` lower/upper_bound and
 *       range erase for good average-case behavior. Further optimizations for
 *       minimal comparisons are possible for very hot paths.
 */
template <IntervalKey K, IntervalValue V>
class IntervalMap {
    /**
     * @class IntervalMap
     *
     * @brief Main container class.
     *
     * Example:
     * @code
     * interval_map::IntervalMap<int, std::string> schedule("Free");
     * schedule.assign(9, 12, "Meeting with Alice");
     * std::cout << schedule[10];   // "Meeting with Alice"
     * @endcode
     */
private:
    using Map = std::map<K, V>;

    V m_valBegin;
    Map m_intervals;

    [[nodiscard]]
    constexpr const V& valueAt(typename Map::const_iterator it) const noexcept {
        return (it == m_intervals.end()) ? m_valBegin : it->second;
    }

    [[nodiscard]]
    constexpr const V& previousValue(typename Map::const_iterator it) const noexcept {
        return (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;
    }

public:
    // --- Construction ---

    IntervalMap() = delete;

    template <ForwardableTo<V> T>
    explicit constexpr IntervalMap(T&& value) noexcept
        : m_valBegin(std::forward<T>(value)) {}

    IntervalMap(const IntervalMap&) = default;
    IntervalMap(IntervalMap&&) noexcept = default;
    IntervalMap& operator=(const IntervalMap&) = default;
    IntervalMap& operator=(IntervalMap&&) noexcept = default;

    // --- Observers ---

    /**
     * @brief Returns a const reference to the underlying change-point map.
     *
     * The returned `std::map` contains only the points where the value changes.
     * This is useful for debugging and for advanced iteration use cases.
     *
     * @warning Do not modify the returned map directly.
     */
    [[nodiscard]] const Map& intervals() const noexcept { return m_intervals; }
    [[nodiscard]] const V& valBegin() const noexcept { return m_valBegin; }

    [[nodiscard]] auto begin() const noexcept { return m_intervals.begin(); }
    [[nodiscard]] auto end() const noexcept { return m_intervals.end(); }

    /**
     * @brief Returns the value associated with a specific key.
     *
     * Finds the greatest change point that is ≤ `key` and returns its value,
     * or the default value if no such point exists.
     *
     * Complexity: O(log n)
     */
    [[nodiscard]] const V& operator[](const K& key) const noexcept {
        auto it = m_intervals.upper_bound(key);
        return (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;
    }

    // --- Modifiers ---

    void swap(IntervalMap& other) noexcept {
        std::swap(m_valBegin, other.m_valBegin);
        m_intervals.swap(other.m_intervals);
    }

    void clear() noexcept {
        m_intervals.clear();
    }

    /**
     * @brief Assigns a value to a half-open interval.
     *
     * Sets the range `[keyBegin, keyEnd)` to the given value.
     * This is the primary mutating operation.
     *
     * - If the value is the same as the one immediately before `keyBegin`,
     *   the new interval is merged with the previous one.
     * - If the value is the same as the one immediately after `keyEnd`,
     *   the following interval is merged.
     * - Intervals completely covered by the new assignment are removed.
     *
     * @tparam T Type convertible/assignable to V (supports perfect forwarding).
     * @param keyBegin Inclusive start of the range.
     * @param keyEnd   Exclusive end of the range.
     * @param val      The value to assign.
     *
     * @note Empty ranges (`keyBegin == keyEnd` or `keyBegin > keyEnd` in ordering)
     *       are silently ignored.
     */
    template <ForwardableTo<V> T>
    void assign(const K& keyBegin, const K& keyEnd, T&& val) {
        if (!(keyBegin < keyEnd)) {
            return;
        }

        // Determine value before the range
        auto it = m_intervals.upper_bound(keyBegin);
        V before = (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;

        // Find first entry at or after keyEnd
        auto itEnd = m_intervals.lower_bound(keyEnd);

        // Erase [keyBegin, keyEnd)
        auto eraseStart = m_intervals.lower_bound(keyBegin);
        m_intervals.erase(eraseStart, itEnd);

        // Insert start if different from 'before'
        if (!(before == val)) {
            m_intervals.emplace(keyBegin, std::forward<T>(val));
        }

        // Determine value after the range (after the erase)
        itEnd = m_intervals.lower_bound(keyEnd);
        V after = (itEnd == m_intervals.end()) ? m_valBegin : itEnd->second;

        // Insert restoration point at keyEnd if needed
        if (!(after == val)) {
            m_intervals.emplace(keyEnd, std::move(after));
        }
    }

    /**
     * @brief Assigns a value from `keyBegin` to the end of the key space.
     *
     * Equivalent to `assign(keyBegin, some_max_value, val)` but more efficient.
     * Everything from `keyBegin` onwards will have the new value (until another
     * `assign` changes it later).
     */
    template <ForwardableTo<V> T>
    void assign(const K& keyBegin, T&& val) {
        // Remove everything from keyBegin onwards
        auto it = m_intervals.lower_bound(keyBegin);
        m_intervals.erase(it, m_intervals.end());

        // Value before keyBegin
        auto prevIt = m_intervals.upper_bound(keyBegin);
        if (prevIt != m_intervals.begin()) --prevIt;

        V before = valueAt(prevIt);

        if (!(before == val)) {
            m_intervals.emplace(keyBegin, std::forward<T>(val));
        }
    }
};

} // namespace interval_map
