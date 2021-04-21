#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/project_defs.h"
#include "utils.h"
#include <memory>
#include <atomic>

namespace reuss {

class UdpSocket;

class Receiver {
    ImageFifo fifo_;
    std::atomic<bool> stopped_{false};
    std::unique_ptr<UdpSocket> sock;

  public:
    Receiver(const std::string &node, const std::string &port);
    Receiver(const std::string &node, const std::string &port,
             size_t fifo_size);
    void receivePackets(int cpu);
    void stop();
    ImageFifo *fifo() { return &fifo_; }
    ~Receiver();
};

} // namespace reuss
