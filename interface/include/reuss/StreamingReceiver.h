#pragma once
#include "reuss/FrameAssembler.h"
#include "reuss/Receiver.h"
#include "reuss/Streamer.h"
#include <string>
#include <vector>

namespace reuss {
class StreamingReceiver {
    std::vector<std::unique_ptr<Receiver>> receivers_;
    std::vector<std::thread> threads_;
    std::unique_ptr<Streamer> streamer_;
    std::unique_ptr<FrameAssembler> assembler_;

    int64_t lost_packets_{};
    int64_t total_frames_{};
    int64_t last_frame_{};

  public:
    StreamingReceiver();
    void start();
    void stop();

    int64_t lost_packets();
    int64_t last_frame();
    int64_t total_frames();

  private:
    struct UdpSources {
        std::string addr;
        std::string port;
    };
    std::vector<UdpSources> get_sources();
};
} // namespace reuss
