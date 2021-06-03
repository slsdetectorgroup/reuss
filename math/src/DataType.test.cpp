

#include "reuss/DataType.h"
#include <catch2/catch.hpp>

using reuss::DataType;

TEST_CASE("Construct from typeid") {
    REQUIRE(DataType(typeid(int)) == typeid(int));
    REQUIRE(DataType(typeid(int)) != typeid(double));
}

TEST_CASE("Construct from string") {
    REQUIRE(DataType("i4") == typeid(int));
    REQUIRE(DataType("i8") == typeid(long));
    REQUIRE(DataType("u4") == typeid(uint32_t));
    REQUIRE(DataType("u8") == typeid(uint64_t));
    REQUIRE(DataType("f4") == typeid(float));
    REQUIRE(DataType("f8") == typeid(double));
}

TEST_CASE("Construct from string with endianess"){
    //TODO! handle big endian system in test!
    REQUIRE(DataType("<i4") == typeid(int32_t));
    REQUIRE_THROWS(DataType(">i4") == typeid(int32_t));
}


TEST_CASE("Convert to string") { REQUIRE(DataType(typeid(int)).str() == "i4"); }

