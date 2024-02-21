#include "reuss/SummingReceiver.h"
#include "reuss/DetectorInterface.h"
#include "reuss/JungfrauDetector.h"
#include <memory>
#include <chrono>
#include <thread>

namespace reuss {
// SummingReceiver::SummingReceiver()
//     : det_(std::make_unique<JungfrauDetector>()) {}

SummingReceiver::SummingReceiver()
    : SummingReceiver(std::make_unique<JungfrauDetector>()) {}

SummingReceiver::SummingReceiver(std::unique_ptr<DetectorInterface> &&d)
    : det_(std::move(d)) {
    try {
        auto sources = det_->get_udp_sources();
        for (auto &s : sources) {
            receivers_.push_back(std::make_unique<Receiver>(s.addr, s.port));
        }

        assembler_ = std::make_unique<FrameAssembler>(receivers_, det_.get(), 8);
        // summer_ = std::make_unique<FrameSummer<float>>(assembler_->fifo(), det_.get());
        for(size_t i=0; i<assembler_->n_fifos(); i++){
            summers_.push_back(std::make_unique<FrameSummer<float>>(assembler_->fifo(i), det_.get()));
        }
        // streamer_ =
        //     std::make_unique<Streamer>(RAW_FRAMES_ENDPOINT, summers_[0]->fifo());

        // int port = 4545;
        // for(size_t i=0; i<assembler_->n_fifos(); i++){
        //     streamers_.push_back(std::make_unique<Streamer>(fmt::format("tcp://*:{}",port++), summers_[i]->fifo()));
        // }
        std::vector<ImageFifo *> fifos;
        for(size_t i=0; i<assembler_->n_fifos(); i++){
            fifos.push_back(summers_[i]->fifo());
        }
        streamer_ = std::make_unique<CollectingStreamer>(RAW_FRAMES_ENDPOINT, fifos);

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}

SummingReceiver::~SummingReceiver() = default;

void SummingReceiver::start() {
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
        // processing_threads_.emplace_back(&FrameSummer<float>::accumulate, summers_[0].get(),
        //                       cpu);
        // cpu+=step;
        // for (auto &s : streamers_){
        //     processing_threads_.emplace_back(&Streamer::stream, s.get(), cpu);
        //     cpu+=step;
        // }
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
    // summer_->stop();
    streamer_->stop();
    // for (auto &s : streamers_){
    //     s->stop();
    // }
    
    
    
    

    for (auto &&t : processing_threads_)
        t.join();

    receiving_threads_.clear();
    processing_threads_.clear();

    // last_frame_ = last_frame();
    // total_frames_ = total_frames();
    // lost_packets_ = lost_packets();


    streamer_ = nullptr;
    summers_.clear();
    assembler_ = nullptr;
    
    //TODO! Look at why we need this sleep!
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    receivers_.clear();
    fmt::print(fg(fmt::color::hot_pink), "SummingReceiver stopped!\n");
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