#include "reuss/DataSpan.h"
#include <catch2/catch.hpp>

#include <iostream>
#include <vector>

TEST_CASE("Element reference 1D") {
    std::vector<int> vec;
    for (int i = 0; i != 10; ++i) {
        vec.push_back(i);
    }
    DataSpan<int, 1> data(vec.data(), Shape<1>{10});
    REQUIRE(vec.size() == data.size());
    for (int i = 0; i != 10; ++i) {
        REQUIRE(data(i) == vec[i]);
        REQUIRE(data[i] == vec[i]);
    }
}

TEST_CASE("Element reference 2D") {
    std::vector<int> vec;
    for (int i = 0; i != 12; ++i) {
        vec.push_back(i);
    }

    DataSpan<int, 2> data(vec.data(), Shape<2>{3, 4});
    REQUIRE(vec.size() == data.size());
    int i = 0;
    for (int row = 0; row != 3; ++row) {
        for (int col = 0; col != 4; ++col) {
            REQUIRE(data(row, col) == i);
            REQUIRE(data[i] == vec[i]);
            ++i;
        }
    }
}

TEST_CASE("Element reference 3D") {
    std::vector<int> vec;
    for (int i = 0; i != 24; ++i) {
        vec.push_back(i);
    }
    DataSpan<int, 3> data(vec.data(), Shape<3>{2, 3, 4});
    REQUIRE(vec.size() == data.size());
    int i = 0;
    for (int frame = 0; frame != 2; ++frame) {
        for (int row = 0; row != 3; ++row) {
            for (int col = 0; col != 4; ++col) {
                REQUIRE(data(frame, row, col) == i);
                REQUIRE(data[i] == vec[i]);
                ++i;
            }
        }
    }
}

TEST_CASE("Plus and miuns with single value") {
    std::vector<int> vec;
    for (int i = 0; i != 12; ++i) {
        vec.push_back(i);
    }
    DataSpan<int, 2> data(vec.data(), Shape<2>{3, 4});
    data += 5;
    int i = 0;
    for (int row = 0; row != 3; ++row) {
        for (int col = 0; col != 4; ++col) {
            REQUIRE(data(row, col) == i + 5);
            ++i;
        }
    }
    data -= 3;
    i = 0;
    for (int row = 0; row != 3; ++row) {
        for (int col = 0; col != 4; ++col) {
            REQUIRE(data(row, col) == i + 2);
            ++i;
        }
    }
}

TEST_CASE("Multiply and divide with single value") {
    std::vector<int> vec;
    for (int i = 0; i != 12; ++i) {
        vec.push_back(i);
    }
    DataSpan<int, 2> data(vec.data(), Shape<2>{3, 4});
    data *= 5;
    int i = 0;
    for (int row = 0; row != 3; ++row) {
        for (int col = 0; col != 4; ++col) {
            REQUIRE(data(row, col) == i * 5);
            ++i;
        }
    }
    data /= 3;
    i = 0;
    for (int row = 0; row != 3; ++row) {
        for (int col = 0; col != 4; ++col) {
            REQUIRE(data(row, col) == (i * 5) / 3);
            ++i;
        }
    }
}

TEST_CASE("elementwise assign"){
    std::vector<int> vec(25);
    DataSpan<int, 2> data(vec.data(), Shape<2>{5,5});

    data = 3;
    for (auto it : data){
        REQUIRE(it == 3);
    }
}

TEST_CASE("iterators") {
    std::vector<int> vec;
    for (int i = 0; i != 12; ++i) {
        vec.push_back(i);
    }
    DataSpan<int, 1> data(vec.data(), Shape<1>{12});
    int i = 0;
    for (const auto item : data) {
        REQUIRE(item == vec[i]);
        ++i;
    }
    REQUIRE(i == 12);

    for (auto ptr = data.begin(); ptr != data.end(); ++ptr) {
        *ptr += 1;
    }
    for (auto& item : data){
        ++item;
    }

    i = 0;
    for (const auto item : data) {
        REQUIRE(item == i + 2);
        ++i;
    }
}


TEST_CASE("shape from vector"){
    std::vector<int> vec;
    for (int i = 0; i != 12; ++i) {
        vec.push_back(i);
    }
    std::vector<ssize_t> shape{3,4};
    DataSpan<int,2> data(vec.data(), shape);
}

TEST_CASE("divide with another span"){
    std::vector<int> vec0{9,12,3};
    std::vector<int> vec1{3,2,1};
    std::vector<int> result{3,6,3};

    DataSpan<int, 1> data0(vec0.data(), Shape<1>{vec0.size()});
    DataSpan<int, 1> data1(vec1.data(), Shape<1>{vec1.size()});

    data0 /= data1;
    
    for(int i =0; i!=vec0.size(); ++i){
        REQUIRE(data0[i] == result[i] );
    }
}

TEST_CASE("Retrive shape"){
    std::vector<int> vec;
    for (int i = 0; i != 12; ++i) {
        vec.push_back(i);
    }
    DataSpan<int,2> data(vec.data(), Shape<2>{3,4});
    REQUIRE(data.shape()[0] == 3);
    REQUIRE(data.shape()[1] == 4);
}