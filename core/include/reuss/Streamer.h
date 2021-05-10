#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/ZmqSocket.h"
#include <atomic>
#include <fmt/format.h>
#include <string>
#include <thread>
namespace reuss {

class Streamer {
    ImageFifo *fifo_;
    ZmqSocket socket_;
    std::atomic<bool> stopped_{false};
    std::atomic<int64_t> last_frame_ = -1;
    std::atomic<int64_t> total_frames_ = 0;

  public:
    Streamer(const std::string &endpoint, ImageFifo *fifo);
    void stream(int cpu);
    void stop() { stopped_ = true; }
    int64_t last_frame(){ return last_frame_; }
    int64_t total_frames(){ return total_frames_;}
};
} // namespace reuss