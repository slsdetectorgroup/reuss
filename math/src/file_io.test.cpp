#include <catch2/catch.hpp>
#include "reuss/file_io.h"
#include <vector>
using reuss::load_numpy_header;
using reuss::DataType;
using reuss::typeid2enum;

// dtype(int)

TEST_CASE("Convert types to enum"){

    REQUIRE(typeid2enum(std::type_index(typeid(int))) == DataType::INT32);
    REQUIRE(typeid2enum(std::type_index(typeid(uint))) == DataType::UINT32);

}


TEST_CASE("Load numpy header"){
    auto h = load_numpy_header("data/np_1d.npy");
    REQUIRE(h.major_ver == 1);
    REQUIRE(h.minor_ver == 0);
    REQUIRE(h.descr.descr == "<i4");
    REQUIRE(h.descr.fortran_order == false);
    REQUIRE(h.descr.shape == std::vector<int>{10});
}

TEST_CASE("Load 1D numpy file"){
    auto data = reuss::load_numpy<int, 1>("data/np_1d.npy");
    REQUIRE(data.size() == 10);
    for (int i = 0; i<10; ++i){
        REQUIRE(data(i) == i);
    }
}

TEST_CASE("Try loading the wrong shape"){
    REQUIRE_THROWS(reuss::load_numpy<int, 3>("data/np_1d.npy"));
}

TEST_CASE("Try loading the wrong type"){
    REQUIRE_THROWS(reuss::load_numpy<float, 1>("data/np_1d.npy"));
}

TEST_CASE("Load a 2d double array"){
    auto data = reuss::load_numpy<double, 2>("data/np_2d.npy");
    REQUIRE(data.shape(0) == 3);
    REQUIRE(data.shape(1) == 4);
    for(int i = 0; i<data.size(); ++i){
        REQUIRE(data(i) == static_cast<double>(i));
    }

    int i = 0;
    for (int row=0; row<data.shape(0); ++row){
        for (int col=0; col<data.shape(1); ++col){
            REQUIRE(data(row, col) == static_cast<double>(i++));
        }
    }

}

TEST_CASE("Parse 1d string"){
    std::string s = "{'descr': '<i4', 'fortran_order': False, 'shape': (10,), }";
    auto r = reuss::NumpyArrDescr(s);
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