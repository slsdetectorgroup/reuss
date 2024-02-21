#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/ZmqSocket.h"
#include <atomic>
#include <fmt/format.h>
#include <string>
#include <thread>
#include <vector>
namespace reuss {

class CollectingStreamer {
    std::vector<ImageFifo*> fifos_;
    ZmqSocket socket_;
    std::atomic<bool> stop_requested_{false};
    std::atomic<bool> stopped_{false};
    std::atomic<int64_t> last_frame_ = -1;
    std::atomic<int64_t> total_frames_ = 0;

  public:
    CollectingStreamer(const std::string &endpoint, std::vector<ImageFifo*> fifos);
    void stream(int cpu);
    void stop() {
        fmt::print(fg(fmt::color::hot_pink), "CollectingStreamer::stop requested\n");
        stop_requested_ = true;
        while (!stopped_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    int64_t last_frame() { return last_frame_; }
    int64_t total_frames() { return total_frames_; }
};
} // namespace reuss