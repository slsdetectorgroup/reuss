#include "reuss/G2Receiver.h"

#include <catch2/catch.hpp>
#include <thread>
using reuss::G2Receiver;



TEST_CASE("Create and set basic properties"){

    G2Receiver r("127.0.0.1", "51230");
    
    REQUIRE(r.done() == false);
    REQUIRE(r.progress() == 0.0);
    REQUIRE(r.fifo() != nullptr);
    REQUIRE(r.preview_fifo() != nullptr);

}

TEST_CASE("stop doesn't hang"){
    G2Receiver r("127.0.0.1", "51230");
    r.stop();
}

