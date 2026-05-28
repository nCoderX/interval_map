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
 * @brief Efficient associative container mapping half-open intervals to values.
 *
 * `IntervalMap<K, V>` associates every key in a half-open interval `[keyBegin, keyEnd)`
 * with a value. Adjacent intervals containing the same value are automatically merged.
 *
 * The default value (provided at construction) is never stored inside the internal map.
 * Only "change points" are stored.
 *
 * This class is inspired by the famous ThinkCell C++ recruitment test.
 *
 * @tparam K Key type. Must be copyable, movable, and support operator<.
 * @tparam V Value type. Must be copyable, movable, and support operator==.
 *
 * @note This implementation focuses on correctness and reasonable efficiency.
 *       Further micro-optimizations for the absolute minimum number of comparisons
 *       are possible.
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

    [[nodiscard]] const Map& intervals() const noexcept { return m_intervals; }
    [[nodiscard]] const V& valBegin() const noexcept { return m_valBegin; }

    [[nodiscard]] auto begin() const noexcept { return m_intervals.begin(); }
    [[nodiscard]] auto end() const noexcept { return m_intervals.end(); }

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
     * @brief Assigns a value to the half-open interval [keyBegin, keyEnd).
     *
     * Sets every key in [keyBegin, keyEnd) to `val`.
     * Adjacent intervals with the same value are automatically merged.
     * The default value is never stored in the map.
     *
     * @param keyBegin Start of the interval (inclusive).
     * @param keyEnd   End of the interval (exclusive).
     * @param val      Value to assign to the range.
     */
    template <ForwardableTo<V> T>
    void assign(const K& keyBegin, const K& keyEnd, T&& val) {
        if (!(keyBegin < keyEnd)) {
            return;
        }

        // Value active just before keyBegin
        auto it = m_intervals.upper_bound(keyBegin);
        V before = (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;

        // Erase everything in [keyBegin, keyEnd)
        auto erase_begin = m_intervals.lower_bound(keyBegin);
        auto erase_end = m_intervals.lower_bound(keyEnd);
        m_intervals.erase(erase_begin, erase_end);

        // Insert start of new interval if different from 'before'
        if (!(before == val)) {
            m_intervals.emplace(keyBegin, std::forward<T>(val));
        }

        // Value that should come after keyEnd
        it = m_intervals.lower_bound(keyEnd);
        V after = (it == m_intervals.end()) ? m_valBegin : it->second;

        // Insert restoration point at keyEnd only if different from new value
        if (!(after == val)) {
            m_intervals.emplace(keyEnd, std::move(after));
        }
    }

    /**
     * @brief Assigns a value from keyBegin to "infinity" (end of the key space).
     */
    template <ForwardableTo<V> T>
    void assign(const K& keyBegin, T&& val) {
        // Remove everything from keyBegin onwards
        auto it = m_intervals.lower_bound(keyBegin);
        m_intervals.erase(it, m_intervals.end());

        // Find what was before keyBegin
        auto prev_it = m_intervals.upper_bound(keyBegin);
        if (prev_it != m_intervals.begin()) {
            --prev_it;
        }

        V prev_val = valueAt(prev_it);

        if (!(prev_val == val)) {
            // We may need to overwrite or insert at keyBegin
            auto at_begin = m_intervals.find(keyBegin);
            if (at_begin != m_intervals.end()) {
                at_begin->second = std::forward<T>(val);
            } else {
                m_intervals.emplace(keyBegin, std::forward<T>(val));
            }
        }
    }
};

} // namespace interval_map
