#include "reuss/StreamingReceiver.h"
#include "reuss/DetectorInterface.h"
#include "reuss/JungfrauDetector.h"
#include <memory>

namespace reuss {
StreamingReceiver::StreamingReceiver()
    : det_(std::make_unique<JungfrauDetector>()) {}

StreamingReceiver::StreamingReceiver(
    std::unique_ptr<DetectorInterface> &&d)
    : det_(std::move(d)) {}

StreamingReceiver::~StreamingReceiver() = default;

void StreamingReceiver::start() {
    try {

        auto sources = det_->get_udp_sources();
        for (auto &s : sources) {
            receivers_.push_back(std::make_unique<Receiver>(s.addr, s.port));
        }

        int cpu = 0;
        for (auto &r : receivers_) {
            threads_.emplace_back(&Receiver::receivePackets, r.get(), cpu++);
        }

        assembler_ = std::make_unique<FrameAssembler>(receivers_);
        threads_.emplace_back(&FrameAssembler::assemble, assembler_.get(),
                              cpu++);

        //insert an accumulator between the assembler and the streamer
        summer_ = std::make_unique<FrameSummer<float>>(assembler_->fifo());
        threads_.emplace_back(&FrameSummer::sum, summer_.get(),
                              cpu++);

        streamer_ = std::make_unique<Streamer>(RAW_FRAMES_ENDPOINT, summer_->fifo());
        threads_.emplace_back(&Streamer::stream, streamer_.get(), cpu++);

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}

void StreamingReceiver::stop() {
    for (auto &r : receivers_)
        r->stop();
    assembler_->stop();
    streamer_->stop();

    for (auto &&t : threads_)
        t.join();

    last_frame_ = last_frame();
    total_frames_ = total_frames();
    lost_packets_ = lost_packets();
    threads_.clear();
    assembler_ = nullptr;
    streamer_ = nullptr;
    receivers_.clear();
    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
}

int64_t StreamingReceiver::lost_packets() {
    if (receivers_.empty())
        return lost_packets_;

    int64_t total = 0;
    for (auto &r : receivers_)
        total += r->lost_packets();
    return total;
}

int64_t StreamingReceiver::last_frame() {
    if (streamer_)
        return streamer_->last_frame();
    else
        return last_frame_;
}

int64_t StreamingReceiver::total_frames() {
    if (streamer_)
        return streamer_->total_frames();
    else
        return -1;
}

} // namespace reuss