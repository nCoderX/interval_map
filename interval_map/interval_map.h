#pragma once

#include <map>
#include <concepts>
#include <type_traits>
#include <utility>

template<typename T>
concept IntevalMapKeyType = std::convertible_to<decltype(std::declval<T>() < std::declval<T>()), bool> 
    && std::copyable<T> 
    && std::is_move_constructible_v<T> 
    && std::is_move_assignable_v<T>;

template<typename T>
concept IntevalMapValueType = std::convertible_to<decltype(std::declval<T>() == std::declval<T>()), bool> 
    && std::copyable<T> 
    && std::is_move_constructible_v<T> 
    && std::is_move_assignable_v<T>;

template<typename T, typename U>
concept ForwardableTo = std::is_constructible_v<U, T> && std::is_assignable_v<U&, T>;

template<IntevalMapKeyType K, IntevalMapValueType V>
class interval_map {
    using interval_map_t = std::map<K, V>;
    V _valBegin;
    interval_map_t _intervalsMap;
    
public:
    interval_map(interval_map const& other) = default;
    interval_map(interval_map&& other) noexcept
        : _valBegin(std::move(other._valBegin)), _intervalsMap(std::move(other._intervalsMap)) {
    }
    template<ForwardableTo<V> T>
    interval_map(T&& val) noexcept
        : _valBegin(std::forward<T>(val)) {
    }
    interval_map& operator=(interval_map const& other) = default;
    interval_map& operator=(interval_map&& other) noexcept {
        _valBegin = std::move(other._valBegin);
        _intervalsMap = std::move(other._intervalsMap);
        return *this;
    }

    [[nodiscard]] typename interval_map_t::iterator begin() const noexcept {
        return _intervalsMap.begin();
    }
    [[nodiscard]] typename interval_map_t::iterator end() const noexcept {
        return _intervalsMap.end();
    }
    [[nodiscard]] const interval_map_t& intervals() const noexcept {
        return _intervalsMap;
    }
    [[nodiscard]] const V& valBegin() const noexcept {
        return _valBegin;
    }
    void swap(interval_map& other) noexcept {
        std::swap(_valBegin, other._valBegin);
        _intervalsMap.swap(other._intervalsMap);
    }
    template<ForwardableTo<V> T>
    void assign(K const& keyBegin, K const& keyEnd, T&& val) noexcept {
        auto itValue = [this](const auto& it) -> const V& { return it == std::end(_intervalsMap) ? _valBegin : it->second; };
        auto previousValue = [this](const auto& it) -> const V& { return it == std::begin(_intervalsMap) ? _valBegin : std::prev(it)->second; };
        if (!(keyBegin < keyEnd)) 
            return;
        auto itOverlapStart = _intervalsMap.upper_bound(keyBegin);
        auto itPrevInterval = (itOverlapStart == std::begin(_intervalsMap)) ? std::end(_intervalsMap) : std::prev(itOverlapStart);
        auto extensionValue = itValue(itPrevInterval);
        auto itNextInterval = std::find_if(itOverlapStart, std::end(_intervalsMap), [&](const auto& interval) {
            if (interval.first < keyEnd || !(keyEnd < interval.first) && interval.second == val) {
                extensionValue = std::move(interval.second);
                return false;
            }
            return true;
        });
        if (itOverlapStart != itNextInterval) 
            itNextInterval = _intervalsMap.erase(itOverlapStart, itNextInterval);
        if (!(extensionValue == val) && (itNextInterval == std::end(_intervalsMap) || keyEnd < itNextInterval->first)) 
            itNextInterval = _intervalsMap.emplace(keyEnd, std::move(extensionValue)).first;
        if (!(itValue(itPrevInterval) == val)) {
            if (itPrevInterval != std::end(_intervalsMap) && !(itPrevInterval->first < keyBegin))
                if (previousValue(itPrevInterval) == val)
                    _intervalsMap.erase(itPrevInterval);
                else
                    itPrevInterval->second = std::forward<T>(val);
            else
                _intervalsMap.emplace(keyBegin, std::forward<T>(val));
        }
    }

    V const& operator[](K const& key) const noexcept {
        auto it = _intervalsMap.upper_bound(key);
        return it == std::begin(_intervalsMap) ? _valBegin : std::prev(it)->second;
    }
};
