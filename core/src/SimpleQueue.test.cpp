
#include "reuss/ImageView.h"
#include "reuss/SimpleQueue.h"
#include <catch2/catch_test_macros.hpp>

// using reuss::SimpleQueue;
using reuss::ImageView;
TEST_CASE("Default constructed queue"){
    SimpleQueue<int> q;
    int a = 5;
    int b = 8;
    q.push(a);
    q.push(b);
    REQUIRE(q.sizeGuess() == 2);

    SimpleQueue<int> q2 = std::move(q);
    REQUIRE(q2.sizeGuess() == 2);
    int c = 0;
    int d = 0;
    q2.pop(c);
    q2.pop(d);
    REQUIRE(c == 5);
    REQUIRE(d == 8);

}