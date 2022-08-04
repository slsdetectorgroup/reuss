#include "UdpSocket.h"
#include <catch2/catch.hpp>

using reuss::UdpSocket;

TEST_CASE("timeout"){
    UdpSocket s("127.0.0.1", "50555", 1024);
    s.receivePacket()
}