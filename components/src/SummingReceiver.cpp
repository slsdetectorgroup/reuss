#include "reuss/SummingReceiver.h"
#include "reuss/DetectorInterface.h"
#include "reuss/JungfrauDetector.h"
#include <memory>

namespace reuss {
// SummingReceiver::SummingReceiver()
//     : det_(std::make_unique<JungfrauDetector>()) {}

SummingReceiver::SummingReceiver()
    : SummingReceiver(std::make_unique<JungfrauDetector>()) {}

SummingReceiver::SummingReceiver(
    std::unique_ptr<DetectorInterface> &&d)
    : det_(std::move(d)) {
        fmt::print("Hej\n");    

    try {
        auto sources = det_->get_udp_sources();
        for (auto &s : sources) {
            receivers_.push_back(std::make_unique<Receiver>(s.addr, s.port));
        }

        assembler_ = std::make_unique<FrameAssembler>(receivers_);
        summer_ = std::make_unique<FrameSummer<float>>(assembler_->fifo());
        streamer_ = std::make_unique<Streamer>(RAW_FRAMES_ENDPOINT, summer_->fifo());


    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }

    }

SummingReceiver::~SummingReceiver() = default;

void SummingReceiver::start() {
    try {
        // auto sources = det_->get_udp_sources();
        // for (auto &s : sources) {
        //     receivers_.push_back(std::make_unique<Receiver>(s.addr, s.port));
        // }

        int cpu = 0;
        for (auto &r : receivers_) {
            threads_.emplace_back(&Receiver::receivePackets, r.get(), cpu++);
        }

        // assembler_ = std::make_unique<FrameAssembler>(receivers_);
        threads_.emplace_back(&FrameAssembler::assemble, assembler_.get(),
                              cpu++);

        //sum frames and stream out
        // summer_ = std::make_unique<FrameSummer<float>>(assembler_->fifo());
        threads_.emplace_back(&FrameSummer<float>::accumulate, summer_.get(),
                              cpu++);

        // streamer_ = std::make_unique<Streamer>(RAW_FRAMES_ENDPOINT, summer_->fifo());
        threads_.emplace_back(&Streamer::stream, streamer_.get(), cpu++);

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}

void SummingReceiver::stop() {
    for (auto &r : receivers_)
        r->stop();
    assembler_->stop();
    summer_->stop();
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

int64_t SummingReceiver::lost_packets() {
    if (receivers_.empty())
        return lost_packets_;

    int64_t total = 0;
    for (auto &r : receivers_)
        total += r->lost_packets();
    return total;
}

int64_t SummingReceiver::last_frame() {
    if (streamer_)
        return streamer_->last_frame();
    else
        return last_frame_;
}

int64_t SummingReceiver::total_frames() {
    if (streamer_)
        return streamer_->total_frames();
    else
        return -1;
}

void SummingReceiver::set_pedestal(ImageData<float, 3> pedestal) {
    summer_->set_pedestal(pedestal);
}

ImageData<float, 3> SummingReceiver::get_pedestal() const{
    return summer_->get_pedestal();
}

void SummingReceiver::set_calibration(ImageData<float, 3> calibration) {
    summer_->set_calibration(calibration);
}

ImageData<float, 3> SummingReceiver::get_calibration() const {
    return summer_->get_calibration();
}

} // namespace reuss