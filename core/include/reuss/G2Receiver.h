#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/project_defs.h"
#include "utils.h"
#include <memory>
#include <atomic>

namespace reuss {

class UdpSocket;

class G2Receiver {
    ImageFifo fifo_;
    std::atomic<bool> stopped_{false};
    std::unique_ptr<UdpSocket> sock;
    std::atomic<int> total_lost_packets_{};

  public:
    G2Receiver(const std::string &node, const std::string &port);
    G2Receiver(const std::string &node, const std::string &port,
             size_t fifo_size);
    void receivePackets(int cpu);
    int lost_packets() const noexcept;
    void stop();
    ImageFifo *fifo() { return &fifo_; }
    ~G2Receiver();
};

} // namespace reuss
