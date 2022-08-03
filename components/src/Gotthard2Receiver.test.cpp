#include "reuss/Gotthard2Receiver.h"

#include <catch2/catch.hpp>
#include <thread>
using reuss::Gotthard2Receiver;


#include <chrono>
using namespace std::literals;
TEST_CASE("emptyu"){

    Gotthard2Receiver r;
    r.setUdpSource("127.0.0.1:51230");
    r.start();
    std::this_thread::sleep_for(1s);
    r.stop();
    // 

}

// TEST_CASE("stop doesn't hang"){
//     G2Receiver r("127.0.0.1", "51230");
//     r.stop();
// }

