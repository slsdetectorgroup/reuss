#include "reuss/SummingReceiver.h"
#include "reuss/DetectorInterface.h"
#include "reuss/JungfrauDetector.h"
#include <memory>
#include <chrono>
#include <thread>

namespace reuss {

SummingReceiver::SummingReceiver(size_t n_assemble_threads)
    : SummingReceiver(std::make_unique<JungfrauDetector>(), n_assemble_threads) {}

SummingReceiver::SummingReceiver(std::unique_ptr<DetectorInterface> &&d, size_t n_assemble_threads)
    : det_(std::move(d)), n_assemble_threads_(n_assemble_threads) {
    arm();
}

void SummingReceiver::arm() {
        try {
        auto sources = det_->get_udp_sources();
        for (auto &s : sources) {
            receivers_.push_back(std::make_unique<Receiver>(s.addr, s.port));
        }

        assembler_ = std::make_unique<FrameAssembler>(receivers_, det_.get(), n_assemble_threads_);
        for(size_t i=0; i<assembler_->n_fifos(); i++){
            summers_.push_back(std::make_unique<FrameSummer<float>>(assembler_->fifo(i), det_.get(), n_assemble_threads_));
        }
        std::vector<ImageFifo *> fifos;
        for(size_t i=0; i<assembler_->n_fifos(); i++){
            fifos.push_back(summers_[i]->fifo());
        }
        streamer_ = std::make_unique<CollectingStreamer>(RAW_FRAMES_ENDPOINT, fifos);

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
    ready = true;
}

SummingReceiver::~SummingReceiver() = default;

void SummingReceiver::start() {
    
    //In case the receiver was stopped and objects destroyed we need
    //to set it up again
    if (!ready)
        arm();

    try {
        int cpu = 0;
        int step = 2;
        for (auto &r : receivers_) {
            receiving_threads_.emplace_back(&Receiver::receivePackets, r.get(), cpu);
            cpu+=step;
        }
        processing_threads_.emplace_back(&FrameAssembler::assemble, assembler_.get(),
                              cpu);
        cpu+=step;

        for(auto &s: summers_){
            processing_threads_.emplace_back(&FrameSummer<float>::accumulate, s.get(),
                              cpu);
            cpu+=step;
        }
        processing_threads_.emplace_back(&CollectingStreamer::stream, streamer_.get(), cpu);

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}

void SummingReceiver::stop() {
    fmt::print(fg(fmt::color::hot_pink), "SummingReceiver stopping...\n");
    
    //Stop the udp receivers
    for (auto &r : receivers_)
        r->stop();

    for (auto &&t : receiving_threads_)
        t.join();
    
    assembler_->stop();
    for (auto &s: summers_){
        s->stop();
    }
    streamer_->stop();
    for (auto &&t : processing_threads_)
        t.join();

    receiving_threads_.clear();
    processing_threads_.clear();

    // Save stats before clearing
    last_frame_ = last_frame();
    total_frames_ = total_frames();
    lost_packets_ = lost_packets();

    streamer_ = nullptr;
    summers_.clear();
    assembler_ = nullptr;

    receivers_.clear();
    fmt::print(fg(fmt::color::hot_pink), "SummingReceiver stopped!\n");
    ready = false;
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
    for (auto &summer_ : summers_){
        summer_->set_pedestal(pedestal);
    }
}

ImageData<float, 3> SummingReceiver::get_pedestal() const {

    return summers_[0]->get_pedestal();
}

void SummingReceiver::set_calibration(ImageData<float, 3> calibration) {
    for (auto &summer_ : summers_){
        summer_->set_calibration(calibration);
    }
}

ImageData<float, 3> SummingReceiver::get_calibration() const {
    return summers_[0]->get_calibration();
}

void SummingReceiver::record_pedestal() {
    assembler_->set_pedestal_mode(true);
    while(assembler_->get_pedestal_mode() == true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    fmt::print("Pedestal recorded\n");
    for(auto &summer_ : summers_){
        summer_->update_pedestal();
    }
}

} // namespace reuss