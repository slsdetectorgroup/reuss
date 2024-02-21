#include "reuss/file_io.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>
using reuss::DataType;



TEST_CASE("Load 1D numpy file") {
    auto data = reuss::load_numpy<int, 1>("data/np_1d.npy");
    REQUIRE(data.size() == 10);
    for (int i = 0; i < 10; ++i) {
        REQUIRE(data(i) == i);
    }
}

TEST_CASE("Throws when loading the wrong shape") {
    REQUIRE_THROWS(reuss::load_numpy<int, 3>("data/np_1d.npy"));
}

TEST_CASE("Throws when loading the wrong type"){
    REQUIRE_THROWS(reuss::load_numpy<float, 1>("data/np_1d.npy"));
}

TEST_CASE("Load a 2d double array") {
    auto data = reuss::load_numpy<double, 2>("data/np_2d.npy");
    REQUIRE(data.shape(0) == 3);
    REQUIRE(data.shape(1) == 4);
    for (int i = 0; i < data.size(); ++i) {
        REQUIRE(data(i) == static_cast<double>(i));
    }

    int i = 0;
    for (int row = 0; row < data.shape(0); ++row) {
        for (int col = 0; col < data.shape(1); ++col) {
            REQUIRE(data(row, col) == static_cast<double>(i++));
        }
    }
}

