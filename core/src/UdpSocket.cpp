#include "UdpSocket.h"
#include <cstring> //memset
#include <fmt/format.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
namespace ur {
UdpSocket::UdpSocket(const std::string &node, const std::string &port,
                     int packet_size)
    : packet_size_(packet_size) {
    // open socket
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    struct addrinfo *res{nullptr};
    if (getaddrinfo(node.c_str(), port.c_str(), &hints, &res))
        throw std::runtime_error("Get getaddrinfo failed");
    sockfd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd_ == -1)
        throw std::runtime_error("Failed to open socket");
    if (bind(sockfd_, res->ai_addr, res->ai_addrlen) == -1) {
        close(sockfd_);
        throw std::runtime_error("Failed to bind socket");
    }
    freeaddrinfo(res);
}

UdpSocket::~UdpSocket() {
    shutdown();
    ::close(sockfd_);
    sockfd_ = -1;
}

void UdpSocket::shutdown() { ::shutdown(sockfd_, SHUT_RDWR); }

bool UdpSocket::receivePacket(void *dst, PacketHeader &header) {
    auto rc = recvfrom(sockfd_, dst, packet_size_, 0, nullptr, nullptr);
    if (rc == packet_size_) {
        memcpy(&header, dst, sizeof(header));
        return true;
    } else {
        fmt::print("Warning: read {} bytes\n", rc);
    }
    return false;
}

void UdpSocket::setBufferSize(size_t size) {
    socklen_t optlen = sizeof(size);
    if (setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, &size, optlen)) {
        throw std::runtime_error("Could not set UDP socket buffer size");
    }
}

size_t UdpSocket::bufferSize() const {
    uint64_t size = 0;
    socklen_t optlen = sizeof(uint64_t);
    getsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, &size, &optlen);
    return size / 2;
}
} // namespace ur