#include "reuss/UdpSocket.h"
#include "reuss/project_defs.h"
#include <catch2/catch.hpp>

#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

using reuss::UdpSocket;
// using reuss::PacketHeader;

int open_socket(int port) {
    const char *host = "127.0.0.1"; // localhost

    // Create a socket for sending
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    struct addrinfo *res = nullptr;

    const std::string portname = std::to_string(port);
    if (getaddrinfo(host, portname.c_str(), &hints, &res)) {
        throw std::runtime_error("Failed at getaddrinfo with " +
                                std::string(host));
    }
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == -1) {
        throw std::runtime_error("Failed to create UDP sending socket");
    }

    if (connect(fd, res->ai_addr, res->ai_addrlen)) {
        throw std::runtime_error("Failed to connect socket");
    }
    freeaddrinfo(res);
    return fd;
}


TEST_CASE("Let the socket timeout and see that we get false and no hang"){
    UdpSocket s("127.0.0.1", "5055", 1024);
    char buffer[1024];
    auto r = s.receivePacket(&buffer[0]);
    REQUIRE(r == false);
}

TEST_CASE("Receive a packet"){

    constexpr size_t data_size = 1024-sizeof(PacketHeader);
    PacketBuffer<data_size> data;
    data.header.frameNumber = 123;

    
    UdpSocket s("127.0.0.1", "50556", sizeof(data));



    int fd = open_socket(50556);
    auto n = write(fd, &data, sizeof(data));
    REQUIRE(n == 1024);

    PacketBuffer<data_size> buf;
    REQUIRE(s.receivePacket(&buf));
    close(fd);

    REQUIRE(buf.header.frameNumber == 123);

}

TEST_CASE("Set buffer size"){
    UdpSocket s("127.0.0.1", "50556", 1024);
    size_t bufs = 8*1024*1024;
    s.setBufferSize(bufs);

    REQUIRE(s.bufferSize() == bufs);

}