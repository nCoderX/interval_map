#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "interval_map/interval_map.hpp"

#include <map>
#include <random>
#include <string>

using namespace interval_map;

// =============================================================================
// Basic functionality
// =============================================================================

TEST_CASE("Basic construction and lookup") {
    IntervalMap<int, char> im('-');

    REQUIRE(im.intervals().empty());
    REQUIRE(im[0] == '-');
    REQUIRE(im[-100] == '-');
    REQUIRE(im[100000] == '-');
}

TEST_CASE("Simple range assignment") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');

    REQUIRE(im[4] == '-');
    REQUIRE(im[5] == 'A');
    REQUIRE(im[9] == 'A');
    REQUIRE(im[10] == '-');
}

TEST_CASE("Overlapping assignments") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');
    im.assign(8, 15, 'B');

    REQUIRE(im[7] == 'A');
    REQUIRE(im[8] == 'B');
    REQUIRE(im[14] == 'B');
    REQUIRE(im[15] == '-');
}

TEST_CASE("Merging adjacent ranges with same value") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');
    im.assign(10, 15, 'A');

    REQUIRE(im[9] == 'A');
    REQUIRE(im[10] == 'A');
}

TEST_CASE("Assigning default value removes entries") {
    IntervalMap<int, char> im('-');
    im.assign(5, 20, 'A');
    im.assign(10, 15, '-');

    REQUIRE(im[9] == 'A');
    REQUIRE(im[10] == '-');
}

TEST_CASE("Full overwrite") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');
    im.assign(10, 15, 'B');
    im.assign(0, 20, 'C');

    REQUIRE(im[0] == 'C');
    REQUIRE(im[19] == 'C');
}

// =============================================================================
// Edge cases
// =============================================================================

TEST_CASE("Zero-length range does nothing") {
    IntervalMap<int, char> im('-');
    im.assign(5, 5, 'X');
    REQUIRE(im.intervals().empty());
}

TEST_CASE("Negative keys") {
    IntervalMap<int, std::string> im("default");
    im.assign(-100, -50, "negative");

    REQUIRE(im[-101] == "default");
    REQUIRE(im[-100] == "negative");
    REQUIRE(im[-51] == "negative");
    REQUIRE(im[-50] == "default");
}

TEST_CASE("Assign from a point to 'infinity'") {
    IntervalMap<int, char> im('-');
    im.assign(10, 20, 'A');
    im.assign(15, 'B');   // from 15 to end

    REQUIRE(im[14] == 'A');
    REQUIRE(im[15] == 'B');
    REQUIRE(im[1000] == 'B');
}

// =============================================================================
// Comprehensive correctness tests
// =============================================================================

TEST_CASE("Complex overlapping scenario") {
    IntervalMap<int, char> im('0');

    im.assign(10, 20, 'A');
    im.assign(15, 25, 'B');
    im.assign(5, 30, 'C');
    im.assign(22, 28, 'D');

    REQUIRE(im[5] == 'C');
    REQUIRE(im[22] == 'D');
}

TEST_CASE("Multiple merges") {
    IntervalMap<int, char> im('-');

    im.assign(0, 10, 'A');
    im.assign(20, 30, 'A');
    im.assign(10, 20, 'A');

    REQUIRE(im[5] == 'A');
    REQUIRE(im[25] == 'A');
}

// =============================================================================
// Performance benchmarks
// =============================================================================

TEST_CASE("Benchmarks") {
    BENCHMARK("10000 sequential small non-overlapping assigns") {
        IntervalMap<int, int> im(0);
        for (int i = 0; i < 10000; ++i) {
            im.assign(i * 10, i * 10 + 5, i);
        }
        return im.intervals().size();
    };

    BENCHMARK("1000 heavily overlapping assigns") {
        IntervalMap<int, int> im(0);
        for (int i = 0; i < 1000; ++i) {
            im.assign(i, i + 500, i % 17);
        }
        return im.intervals().size();
    };

    BENCHMARK("5000 random overlapping assigns") {
        IntervalMap<int, int> im(-1);
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, 10000);

        for (int i = 0; i < 5000; ++i) {
            int a = dist(rng);
            int b = a + dist(rng) % 200 + 1;
            im.assign(a, b, i % 100);
        }
        return im.intervals().size();
    };
}

// =============================================================================
// Additional edge case coverage
// =============================================================================

TEST_CASE("Assign same value as existing previous") {
    IntervalMap<int, char> im('-');
    im.assign(0, 10, 'A');
    im.assign(10, 20, 'A');   // should merge
    REQUIRE(im.intervals().size() == 1);
    REQUIRE(im[5] == 'A');
    REQUIRE(im[15] == 'A');
}

TEST_CASE("Assign value that matches the one after the range") {
    IntervalMap<int, char> im('-');
    im.assign(20, 30, 'A');
    im.assign(10, 20, 'A');   // should merge with the following
    REQUIRE(im.intervals().size() == 1);
}

TEST_CASE("Multiple overlapping in complex order") {
    IntervalMap<int, char> im('0');
    im.assign(0, 100, 'X');
    im.assign(10, 20, 'A');
    im.assign(15, 25, 'B');
    im.assign(5, 30, 'C');
    REQUIRE(im[4] == 'X');
    REQUIRE(im[6] == 'C');
    REQUIRE(im[29] == 'C');
    REQUIRE(im[30] == 'X');
}

TEST_CASE("Assign touching but not overlapping") {
    IntervalMap<int, char> im('-');
    im.assign(0, 10, 'A');
    im.assign(10, 20, 'B');
    REQUIRE(im.intervals().size() == 2);
    REQUIRE(im[9] == 'A');
    REQUIRE(im[10] == 'B');
}

TEST_CASE("Re-assigning exact same range with same value") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');
    im.assign(5, 10, 'A');
    REQUIRE(im.intervals().size() == 1);
}

TEST_CASE("Assigning over default multiple times") {
    IntervalMap<int, char> im('-');
    im.assign(0, 100, 'X');
    im.assign(10, 90, '-');     // punch hole back to default
    REQUIRE(im.intervals().size() == 2);
    REQUIRE(im[5] == 'X');
    REQUIRE(im[50] == '-');
    REQUIRE(im[95] == 'X');
}

