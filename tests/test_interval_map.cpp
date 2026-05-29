#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "interval_map/interval_map.hpp"

#include <random>
#include <string>

using namespace interval_map;

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
    im.assign(15, 'B');

    REQUIRE(im[14] == 'A');
    REQUIRE(im[15] == 'B');
    REQUIRE(im[1000] == 'B');
}

TEST_CASE("Benchmark: many small inserts") {
    BENCHMARK("10000 sequential small assigns") {
        IntervalMap<int, int> im(0);
        for (int i = 0; i < 10000; ++i) {
            im.assign(i * 10, i * 10 + 5, i);
        }
        return im.intervals().size();
    };
}

TEST_CASE("Benchmark: heavy overlapping") {
    BENCHMARK("1000 overlapping assigns") {
        IntervalMap<int, int> im(0);
        for (int i = 0; i < 1000; ++i) {
            im.assign(i, i + 500, i % 17);
        }
        return im.intervals().size();
    };
}

TEST_CASE("Benchmark: random workload") {
    BENCHMARK("5000 random assigns on [0,10000]") {
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
