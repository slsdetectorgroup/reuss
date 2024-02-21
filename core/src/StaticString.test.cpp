#include "reuss/StaticString.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>

using reuss::StaticString;

TEST_CASE("Default construction"){
    StaticString<10> s;
    REQUIRE(s.size() == 0UL);
    REQUIRE(s == "");
}

TEST_CASE("Assign from char[]"){
    char arr[] = "some string";
    StaticString<20> s;
    s = arr;
    REQUIRE(s == arr);
    REQUIRE(s.size() == strlen(arr));
}

TEST_CASE("Construct from char[]"){
    StaticString<10> s{"hej"};
    REQUIRE(s == "hej");
    REQUIRE(s.size() == 3);
    REQUIRE(s[4] == '\0');
}

TEST_CASE("Clear StaticString"){
    StaticString<15> s{"oups"};
    s.clear();
    REQUIRE(s.size() == 0);
    REQUIRE(s == "");
}