// Copyright (c) 2024
// All rights reserved.
//
// This code is licensed under the MIT License.
// You may obtain a copy of the License at
// https://opensource.org/licenses/MIT

#pragma once

#include <map>
#include <concepts>
#include <type_traits>
#include <utility>
#include <cassert>

namespace interval_map {

template <typename T>
concept IntervalKey = std::copyable<T> &&
                      std::is_move_constructible_v<T> &&
                      std::is_move_assignable_v<T> &&
                      requires(T a, T b) {
                          { a < b } -> std::convertible_to<bool>;
                      };

template <typename T>
concept IntervalValue = std::copyable<T> &&
                        std::is_move_constructible_v<T> &&
                        std::is_move_assignable_v<T> &&
                        requires(T a, T b) {
                            { a == b } -> std::convertible_to<bool>;
                        };

template <typename T, typename U>
concept ForwardableTo = std::is_constructible_v<U, T> && std::is_assignable_v<U&, T>;

/**
 * @brief Associative container that maps half-open intervals [keyBegin, keyEnd) to values.
 *
 * Adjacent intervals with the same value are automatically merged.
 * The default value (valBegin) is never stored explicitly in the map.
 *
 * This is a modernized and improved implementation inspired by the
 * famous ThinkCell C++ recruitment test.
 */
template <IntervalKey K, IntervalValue V>
class IntervalMap {
private:
    using MapType = std::map<K, V>;

    V m_valBegin;
    MapType m_intervals;

    [[nodiscard]]
    constexpr const V& getValueAtIterator(typename MapType::const_iterator it) const noexcept {
        return (it == m_intervals.end()) ? m_valBegin : it->second;
    }

    [[nodiscard]]
    constexpr const V& getPreviousValue(typename MapType::const_iterator it) const noexcept {
        return (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;
    }

public:
    // --- Constructors ---

    IntervalMap() = delete;

    explicit IntervalMap(V const& val) : m_valBegin(val) {}
    explicit IntervalMap(V&& val) noexcept : m_valBegin(std::move(val)) {}

    IntervalMap(IntervalMap const& other) = default;
    IntervalMap(IntervalMap&& other) noexcept = default;

    IntervalMap& operator=(IntervalMap const& other) = default;
    IntervalMap& operator=(IntervalMap&& other) noexcept = default;

    // --- Observers ---

    [[nodiscard]] const MapType& intervals() const noexcept { return m_intervals; }
    [[nodiscard]] const V& valBegin() const noexcept { return m_valBegin; }

    [[nodiscard]] typename MapType::const_iterator begin() const noexcept {
        return m_intervals.begin();
    }

    [[nodiscard]] typename MapType::const_iterator end() const noexcept {
        return m_intervals.end();
    }

    [[nodiscard]] V const& operator[](K const& key) const noexcept {
        auto it = m_intervals.upper_bound(key);
        return (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;
    }

    // --- Modifiers ---

    void swap(IntervalMap& other) noexcept {
        std::swap(m_valBegin, other.m_valBegin);
        m_intervals.swap(other.m_intervals);
    }

    /**
     * @brief Assigns a value to the half-open interval [keyBegin, keyEnd).
     */
    template <ForwardableTo<V> T>
    void assign(K const& keyBegin, K const& keyEnd, T&& val) {
        if (!(keyBegin < keyEnd)) {
            return;
        }

        // Find the first point strictly after keyBegin
        auto it = m_intervals.upper_bound(keyBegin);

        // Value just before keyBegin
        V prevVal = (it == m_intervals.begin()) ? m_valBegin : std::prev(it)->second;

        // Remove all intervals that start inside [keyBegin, keyEnd)
        while (it != m_intervals.end() && it->first < keyEnd) {
            prevVal = it->second; // last overwritten value
            it = m_intervals.erase(it);
        }

        // Insert start of new interval if necessary
        bool needStart = (prevVal != val);
        if (needStart) {
            // Check if previous entry can be overwritten
            auto prev = m_intervals.upper_bound(keyBegin);
            if (prev != m_intervals.begin()) {
                --prev;
                if (prev->first == keyBegin) {
                    prev->second = std::forward<T>(val);
                    needStart = false;
                }
            }
            if (needStart) {
                m_intervals.emplace(keyBegin, std::forward<T>(val));
            }
        }

        // Insert restoration point at keyEnd if needed
        V afterVal = (it == m_intervals.end()) ? m_valBegin : it->second;
        if (afterVal != val) {
            // Only insert if there's not already an entry at keyEnd
            auto atEnd = m_intervals.find(keyEnd);
            if (atEnd == m_intervals.end()) {
                m_intervals.emplace(keyEnd, afterVal);
            }
        }
    }

    /**
     * @brief Assigns a value from keyBegin to the end (convenience overload).
     */
    template <ForwardableTo<V> T>
    void assign(K const& keyBegin, T&& val) {
        // Simplified version: assign from keyBegin to "infinity"
        auto it = m_intervals.upper_bound(keyBegin);
        m_intervals.erase(it, m_intervals.end());

        auto prevIt = m_intervals.upper_bound(keyBegin);
        if (prevIt != m_intervals.begin()) {
            --prevIt;
        }

        if (getValueAtIterator(prevIt) != val) {
            if (prevIt != m_intervals.end() && prevIt->first == keyBegin) {
                prevIt->second = std::forward<T>(val);
            } else {
                m_intervals.emplace(keyBegin, std::forward<T>(val));
            }
        }
    }

    void clear() {
        m_intervals.clear();
    }
};

} // namespace interval_map
