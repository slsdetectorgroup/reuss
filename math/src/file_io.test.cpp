#include <catch2/catch.hpp>
#include "reuss/file_io.h"
#include <vector>
using reuss::load_numpy_header;

TEST_CASE("Load"){
    auto h = load_numpy_header("data/np_1d.npy");
    
}

TEST_CASE("Parse 1d string"){
    std::string s = "{'descr': '<i4', 'fortran_order': False, 'shape': (10,), }";
    auto r = reuss::parse_numpy_descr(s);
    REQUIRE(r.descr == "<i4");
    REQUIRE(r.fortran_order == false);
    REQUIRE(r.shape == std::vector<int>{10});
}

TEST_CASE("find_between"){
    std::string s = "some text: 'value', and some more text";
    auto r = reuss::find_between('\'', s);
    REQUIRE(r == "value");
}

TEST_CASE("find_between ()"){
    std::string s = "some text with a (value) between ( and )";
    auto r = reuss::find_between('(', ')', s);
    REQUIRE(r == "value");
}

TEST_CASE("parse empty string"){
    std::string s;
    REQUIRE(reuss::str2vec(s).empty());
}

TEST_CASE("parse single element vec"){
    std::string s = "(50,)";
    REQUIRE(reuss::str2vec(s)==std::vector<int>{50});
}

TEST_CASE("parse two element vec"){
    std::string s = "(789, 788,)";
    REQUIRE(reuss::str2vec(s)==std::vector<int>{789, 788});
}

TEST_CASE("parse three element vec"){
    std::string s = "(1, 2, 3)";
    REQUIRE(reuss::str2vec(s)==std::vector<int>{1, 2, 3});
}