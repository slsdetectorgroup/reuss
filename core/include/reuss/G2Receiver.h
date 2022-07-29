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
    ImageFifo preview_fifo_;
    std::atomic<bool> stopped_{false};
    std::unique_ptr<UdpSocket> sock;
    std::atomic<int> total_lost_packets_{};

    std::size_t n_frames;

  public:
    G2Receiver(const std::string &node, const std::string &port);
    G2Receiver(const std::string &node, const std::string &port,
             size_t fifo_size, size_t n_frames);
    void receivePackets(int cpu);
    int lost_packets() const noexcept;
    void stop();
    ImageFifo *fifo() { return &fifo_; }
    ImageFifo *preview_fifo() { return &preview_fifo_; }

    ~G2Receiver();
};

} // namespace reuss
