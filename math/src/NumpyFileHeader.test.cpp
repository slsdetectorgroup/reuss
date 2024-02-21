#include "reuss/NumpyFileHeader.h"

#include <catch2/catch_test_macros.hpp>

using reuss::NumpyFileHeader;

TEST_CASE("Load numpy header from file") {
    auto h = NumpyFileHeader::fromFile("data/np_1d.npy");
    REQUIRE(h.major_ver() == 1);
    REQUIRE(h.minor_ver() == 0);
    REQUIRE(h.dtype() == "<i4");
    REQUIRE(h.fortran_order() == false);
    REQUIRE(h.shape() == std::vector<int>{10});
}

TEST_CASE("Numpy file header from DataType and shape"){
    NumpyFileHeader h(reuss::DataType(typeid(int)), {10,15});

    REQUIRE(h.fortran_order() == false);
    REQUIRE(h.shape() == std::vector<int>{10,15});
}

TEST_CASE("Parse 1d string") {
    std::string s =
        "{'descr': '<i4', 'fortran_order': False, 'shape': (10,), }";
    auto r = reuss::NumpyArrDescr(s);
    REQUIRE(r.descr() == "<i4");
    REQUIRE(r.fortran_order() == false);
    REQUIRE(r.shape() == std::vector<int>{10});

    REQUIRE(r.str() == s);
}

TEST_CASE("find_between") {
    std::string s = "some text: 'value', and some more text";
    auto r = reuss::find_between('\'', s);
    REQUIRE(r == "value");
}

TEST_CASE("Find text between :"){
    std::string s = "Here is some: text with: in it";
    auto r = reuss::find_between(':', s);
    REQUIRE(r == " text with");

}

TEST_CASE("find_between ()") {
    std::string s = "some text with a (value) between ( and )";
    auto r = reuss::find_between('(', ')', s);
    REQUIRE(r == "value");
}

TEST_CASE("parse empty string") {
    std::string s;
    REQUIRE(reuss::str2vec(s).empty());
}

TEST_CASE("parse single element vec") {
    std::string s = "(50,)";
    REQUIRE(reuss::str2vec(s) == std::vector<int>{50});
}

TEST_CASE("parse two element vec") {
    std::string s = "(789, 788,)";
    REQUIRE(reuss::str2vec(s) == std::vector<int>{789, 788});
}

TEST_CASE("parse three element vec") {
    std::string s = "(1, 2, 3)";
    REQUIRE(reuss::str2vec(s) == std::vector<int>{1, 2, 3});
}

TEST_CASE("vec to string"){
    std::vector<int> vec{1,2,3};
    REQUIRE(reuss::vec2str(vec) == "(1, 2, 3,)");
}

TEST_CASE("vec to string 2"){
    std::vector<int> vec{379, 1};
    REQUIRE(reuss::vec2str(vec) == "(379, 1,)");
}

TEST_CASE("empty vector to string"){
    std::vector<int> vec;
    REQUIRE(reuss::vec2str(vec) == "()");
}