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
 * @brief Efficient map from half-open intervals [keyBegin, keyEnd) to values.
 *
 * This is a high-performance implementation suitable for the ThinkCell-style
 * interview challenge. Adjacent intervals with identical values are merged
 * automatically. The initial value is never stored in the underlying map.
 */
template <IntervalKey K, IntervalValue V>
class IntervalMap {
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
     * @brief Assigns `val` to the half-open interval [keyBegin, keyEnd).
     *
     * This is the core operation. It is designed to be efficient (few
     * comparisons and value copies) while maintaining the invariants.
     */
    template <ForwardableTo<V> T>
    void assign(const K& keyBegin, const K& keyEnd, T&& val) {
        if (!(keyBegin < keyEnd))
            return;

        auto itOverlapStart = m_intervals.upper_bound(keyBegin);
        auto itPrevInterval = (itOverlapStart == m_intervals.begin())
                                  ? m_intervals.end()
                                  : std::prev(itOverlapStart);
        V extensionValue = valueAt(itPrevInterval);

        auto itNextInterval = std::find_if(
            itOverlapStart, m_intervals.end(), [&](const auto& interval) {
                if (interval.first < keyEnd ||
                    (!(keyEnd < interval.first) && interval.second == val)) {
                    extensionValue = interval.second;
                    return false;
                }
                return true;
            });

        if (itOverlapStart != itNextInterval) {
            itNextInterval = m_intervals.erase(itOverlapStart, itNextInterval);
        }

        if (!(extensionValue == val) &&
            (itNextInterval == m_intervals.end() || keyEnd < itNextInterval->first)) {
            itNextInterval = m_intervals.emplace(keyEnd, std::move(extensionValue)).first;
        }

        if (!(valueAt(itPrevInterval) == val)) {
            if (itPrevInterval != m_intervals.end() &&
                !(itPrevInterval->first < keyBegin)) {
                if (previousValue(itPrevInterval) == val) {
                    m_intervals.erase(itPrevInterval);
                } else {
                    itPrevInterval->second = std::forward<T>(val);
                }
            } else {
                m_intervals.emplace(keyBegin, std::forward<T>(val));
            }
        }
    }

    template <ForwardableTo<V> T>
    void assign(const K& keyBegin, T&& val) {
        auto itOverlapStart = m_intervals.upper_bound(keyBegin);
        auto itPrevInterval = (itOverlapStart == m_intervals.begin())
                                  ? m_intervals.end()
                                  : std::prev(itOverlapStart);

        if (itOverlapStart != m_intervals.end()) {
            m_intervals.erase(itOverlapStart, m_intervals.end());
        }

        if (!(valueAt(itPrevInterval) == val)) {
            if (itPrevInterval != m_intervals.end() &&
                !(itPrevInterval->first < keyBegin)) {
                if (previousValue(itPrevInterval) == val) {
                    m_intervals.erase(itPrevInterval);
                } else {
                    itPrevInterval->second = std::forward<T>(val);
                }
            } else {
                m_intervals.emplace(keyBegin, std::forward<T>(val));
            }
        }
    }
};

} // namespace interval_map
