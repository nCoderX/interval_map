#include <catch2/catch_test_macros.hpp>
#include "interval_map/interval_map.hpp"

using namespace interval_map;

TEST_CASE("Basic construction and lookup") {
    IntervalMap<int, char> im('-');

    REQUIRE(im.intervals().empty());
    REQUIRE(im[0] == '-');
    REQUIRE(im[100] == '-');
}

TEST_CASE("Basic assign creates one entry") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');

    REQUIRE(im[4] == '-');
    REQUIRE(im[5] == 'A');
    REQUIRE(im[9] == 'A');
    REQUIRE(im[10] == '-');
}

TEST_CASE("Overlapping assign") {
    IntervalMap<int, char> im('-');
    im.assign(5, 10, 'A');
    im.assign(8, 15, 'B');

    REQUIRE(im[7] == 'A');
    REQUIRE(im[8] == 'B');
    REQUIRE(im[14] == 'B');
    REQUIRE(im[15] == '-');
}
