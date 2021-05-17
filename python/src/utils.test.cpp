#include <catch2/catch.hpp>
#include "utils.h"
#include <vector>

TEST_CASE("min of 0 elements"){
    double val = 33.;
    REQUIRE(arr_min(&val, 0) == 0); //Doesn't make mathematical sense but ok for pedestals
}

TEST_CASE("max of 0 elements"){
    double val = 13.;
    REQUIRE(arr_max(&val, 0) == 0); //Doesn't make mathematical sense but ok for pedestals
}

TEST_CASE("min of one element"){
    double val = 4.2;
    REQUIRE(arr_min(&val, 1) == 4.2);
}

TEST_CASE("min of vector"){
    std::vector<double> values{-1,1, 2, 5, -10, 100, 43.};
    REQUIRE(arr_min(values.data(), values.size())== -10.0);
}

TEST_CASE("max of one element"){
    double val = 7.7;
    REQUIRE(arr_max(&val, 1) == 7.7);
}

TEST_CASE("max of vector"){
    std::vector<double> values{-12., 12.3, 14, 18., -300};
    REQUIRE(arr_max(values.data(), values.size()) == 18.);
}