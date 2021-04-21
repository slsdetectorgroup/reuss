#pragma once
#include "reuss/project_defs.h"
#include <string>
namespace ur {
class UdpSocket {
    int sockfd_ = -1;
    int packet_size_;

  public:
    UdpSocket(const std::string &node, const std::string &port,
              int packet_size);
    ~UdpSocket();
    bool receivePacket(void *dst, PacketHeader &header);
    void setBufferSize(size_t size);
    size_t bufferSize() const;
    void shutdown();
};
} // namespace ur