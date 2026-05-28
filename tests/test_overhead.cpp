#include <catch2/catch_test_macros.hpp>
#include "interval_map/interval_map.hpp"

#include <stdexcept>

// This file contains strict overhead tests similar to the original ThinkCell challenge.
// The goal is to ensure that assign() does not perform unnecessary copies or comparisons.

namespace {

bool g_testOverhead = true;

class StrictKey {
    int m_value;
    mutable int m_compCount = 0;
    mutable int m_assignCount = 0;

    void checkAssignment() const {
        if (g_testOverhead && m_assignCount > 0) {
            throw std::runtime_error("Too many assignments on StrictKey");
        }
        ++m_assignCount;
    }

    void checkComparison() const {
        if (g_testOverhead && m_compCount > 1) {
            throw std::runtime_error("Too many comparisons on StrictKey");
        }
        ++m_compCount;
    }

public:
    StrictKey() = delete;

    explicit StrictKey(int v) : m_value(v) { checkAssignment(); }

    StrictKey(const StrictKey& other) : m_value(other.m_value) { checkAssignment(); }
    StrictKey(StrictKey&& other) noexcept : m_value(other.m_value) {}

    StrictKey& operator=(const StrictKey& other) {
        checkAssignment();
        m_value = other.m_value;
        return *this;
    }

    StrictKey& operator=(StrictKey&& other) noexcept {
        m_value = other.m_value;
        return *this;
    }

    bool operator<(const StrictKey& other) const {
        checkComparison();
        return m_value < other.m_value;
    }

    int value() const { return m_value; }
};

class StrictValue {
    char m_value;
    mutable int m_compCount = 0;
    mutable int m_assignCount = 0;

    void checkAssignment() const {
        if (g_testOverhead && m_assignCount > 0) throw std::runtime_error("Too many value assignments");
        ++m_assignCount;
    }
    void checkComparison() const {
        if (g_testOverhead && m_compCount > 0) throw std::runtime_error("Too many value comparisons");
        ++m_compCount;
    }

public:
    StrictValue() = delete;
    explicit StrictValue(char v) : m_value(v) { checkAssignment(); }

    StrictValue(const StrictValue& other) : m_value(other.m_value) { checkAssignment(); }
    StrictValue(StrictValue&& other) noexcept : m_value(other.m_value) {}

    StrictValue& operator=(const StrictValue& other) {
        checkAssignment();
        m_value = other.m_value;
        return *this;
    }
    StrictValue& operator=(StrictValue&& other) noexcept {
        m_value = other.m_value;
        return *this;
    }

    bool operator==(const StrictValue& other) const {
        checkComparison();
        return m_value == other.m_value;
    }

    char value() const { return m_value; }
};

} // anonymous namespace

TEST_CASE("Strict overhead - basic assign") {
    using IM = interval_map::IntervalMap<StrictKey, StrictValue>;

    g_testOverhead = true;
    IM im(StrictValue{'-'});

    // This should succeed with very few operations
    im.assign(StrictKey{10}, StrictKey{20}, StrictValue{'A'});

    g_testOverhead = false;
    REQUIRE(im[StrictKey{5}].value() == '-');
    REQUIRE(im[StrictKey{15}].value() == 'A');
}
