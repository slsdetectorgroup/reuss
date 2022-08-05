#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/project_defs.h"
#include "reuss/UdpSocket.h"
#include "utils.h"
#include <memory>
#include <atomic>

namespace reuss {



class G2UdpReceiver {
    ImageFifo fifo_;
    ImageFifo preview_fifo_;
    UdpSocket sock;
    std::atomic<bool> stopped_{false};    
    std::atomic<int> total_lost_packets_{};
    std::atomic<bool> done_{false};
    std::atomic<double> progress_{};
    
  public:
    G2UdpReceiver(const std::string &node, const std::string &port);
    G2UdpReceiver(const std::string &node, const std::string &port,
             size_t fifo_size);
    void receive_n(int cpu, size_t n_frames, size_t stream_nth);
    int lost_packets() const noexcept;
    void stop();
    bool done() const;
    double progress() const;
    ImageFifo *fifo() { return &fifo_; }
    ImageFifo *preview_fifo() { return &preview_fifo_; }

    ~G2UdpReceiver();
};

} // namespace reuss
