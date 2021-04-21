#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/ZmqSocket.h"
#include <atomic>
#include <fmt/format.h>
#include <string>
#include <thread>
namespace ur {

class Streamer {
    ImageFifo *fifo_;
    ZmqSocket socket_;
    std::atomic<bool> stopped_{false};

  public:
    Streamer(const std::string &endpoint, ImageFifo *fifo);
    void stream(int cpu);
    void stop() { stopped_ = true; }
};
} // namespace ur