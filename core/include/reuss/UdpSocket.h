#pragma once
#include "reuss/project_defs.h"
#include <string>
namespace reuss {
class UdpSocket {
    int sockfd_ = -1;
    size_t packet_size_;

  public:
    UdpSocket(const std::string &node, const std::string &port,
              size_t packet_size);
    ~UdpSocket();
    bool receivePacket(void *dst);
    void setBufferSize(size_t size);
    size_t bufferSize() const;
    void shutdown();

    // No copy since the class manage the underlying socket
    UdpSocket(const UdpSocket &) = delete;
    UdpSocket &operator=(UdpSocket const &) = delete;

};
} // namespace reuss