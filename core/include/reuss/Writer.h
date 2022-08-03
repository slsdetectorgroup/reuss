#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/ZmqSocket.h"
#include <atomic>
#include <fmt/format.h>
#include <string>
#include <thread>
#include <filesystem>
namespace reuss {

class Writer {
    ImageFifo *fifo_;
    std::atomic<bool> stopped_{false};
    std::atomic<int64_t> last_frame_ = -1;
    std::atomic<int64_t> total_frames_ = 0;
    size_t n_frames{};
    std::string pathname;


  public:
    Writer(ImageFifo *fifo, size_t n_frames, std::string_view pathname);
    void write(int cpu);
    void sink(int cpu);
    void stop();
    int64_t last_frame() const;
    int64_t total_frames() const;
};
} // namespace reuss